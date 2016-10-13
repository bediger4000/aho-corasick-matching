#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <ac7.h>

/* Queue-as-circular-buffer data structure. */
#define CBUFSIZE 0x200  /* 512 */
#define CBUFMASK 0x1ff  /* 512 - 1 */

struct queue_node {
	int state;
	struct queue_node *next;
};

struct queue {
	int *cbuf;
	int  in;
	int  out;
	struct queue_node *ovfhead;
	struct queue_node *ovftail;
};


struct queue *queueinit(void);
void queuedestroy(struct queue *);
void enqueue(struct queue *, int);
int  dequeue(struct queue *);
int  queueempty(struct queue *);
void empty_error(struct queue *);


struct gto *
init_goto()
{
	int i;
	struct gto *g = NULL;

	g = malloc(sizeof(*g));

	g->ary = malloc(sizeof(int *));

	g->ary[0] = calloc(128, sizeof(int));

	g->ary_len = 1;

	for (i = 0; i < 128; ++i)
		g->ary[0][i] = -1;

	g->accept = NULL;
	g->accept_len = 0;

	return g;
}

void
construct_goto(char *keywords[], int keyword_count, struct gto *g)
{
	int newstate = 0;
	int i;

	for (i = 0; i < keyword_count; ++i)
	{
		int state = 0, j = 0, p;

		/* procedure enter() */

		while (
			FAIL != (
				(state<g->ary_len) ?
					g->ary[state][(int)keywords[i][j]] :
					(assert(state>=g->ary_len),FAIL)
				)
			)
		{
			state = ((state<g->ary_len) ?
						g->ary[state][(int)keywords[i][j]] :
						(assert(state>=g->ary_len),FAIL)
					);
			++j;
		}

		for (p = j; '\0' != keywords[i][p]; ++p)
		{
			++newstate;
			add_state(g, state, keywords[i][p], newstate);
			state = newstate;
		}

		/* end procedure enter() */

		set_accept(g, state, keywords[i]);
	}

	for (i = 0; i < 128; ++i)
	{
		if (FAIL == g->ary[0][i])
			g->ary[0][i] = 0;
	}
}

void
set_accept(struct gto *p, int state, char *output)
{
	struct output *n;

	if (state >= p->accept_len)
	{
		int i, n = state - p->accept_len + 1;

		/* Convoluted because when and where I wrote this, realloc(NULL)
		 * caused a segfault or something. */
		p->accept = ((NULL == p->accept) ?
			malloc(n*sizeof(*p->accept)) :
			(char *)realloc(p->accept, (p->accept_len+n)*sizeof(*p->accept))
		);

		i = p->accept_len;
		p->accept_len += n;

		for (; i < p->accept_len; ++i)
			p->accept[i].next = NULL;

		assert(state <= p->accept_len);
	}

	n = (struct output *)malloc(sizeof(*n));

	n->output = malloc(strlen(output)+1);

	strcpy(n->output, output);

	n->next = p->accept[state].next;
	p->accept[state].next = n;
}


void
add_state(struct gto *p, int state, char input, int new_state)
{
	assert(NULL != p);
	assert(FAIL != state);
	assert('\0' != input);
	assert(0 != new_state);
	assert(FAIL != new_state);

	if (state >= p->ary_len || new_state >= p->ary_len)
	{
		int i, n;

		n = 1 + ((new_state > state? new_state: state) - p->ary_len);

		p->ary = (int **)realloc(p->ary, sizeof(int *)*(p->ary_len + n));
		assert(NULL != p->ary);

		for (i = p->ary_len; i < p->ary_len + n; ++i)
		{
			int j;

			p->ary[i] = (int *)malloc(sizeof(int)*128);
			assert(NULL != p->ary[i]);

			for (j = 0; j < 128; ++j)
				p->ary[i][j] = FAIL;
		}

		p->ary_len += n;
	}

	assert(state < p->ary_len);
	p->ary[state][(int)input] = new_state;
}

void
construct_failure(struct gto *g)
{
	int i;
	struct queue *q;

	assert(NULL != g);

	g->failure = (int *)malloc(g->ary_len*sizeof(int));
	assert(NULL != g->failure);

	for (i = 0; i < g->ary_len; ++i)
		g->failure[i] = 0;

	q = queueinit();

	for (i = 0; i < 128; ++i)
	{
		int s = g->ary[0][i];

		if (0 != s)
		{
			enqueue(q, s);
			g->failure[s] = 0;
		}
	}

	while (!queueempty(q))
	{
		int a;
		int r = dequeue(q);

		for (a = 0; a < 128; ++a)
		{
			int s = g->ary[r][a];

			if (FAIL != s)
			{
				int            state;
				struct output *t;

				enqueue(q, s);

				state = g->failure[r];
				while (FAIL == g->ary[state][a])
					state = g->failure[state];

				g->failure[s] = g->ary[state][a];

				/* output(s) <- output(s) U output(f(s)) */

				t = g->accept[g->failure[s]].next;
				while (NULL != t)
				{
					set_accept(g, s, t->output);
					t = t->next;
				}
			}
		}
	}

	queuedestroy(q);
}

int
perform_match(struct gto *g, char *in)
{
	int state = 0;
	int match = 0;
	char *s = in;

	while(*in != '\0')
	{
		if (isascii(*in))
		{
			int nextstate;
			nextstate = g->delta[state][(int)*in];
			state = nextstate;
		}
		++in;
	}

	if (g->accept[state].next)
	{
		struct output *t = g->accept[state].next;

		while (!match && NULL != t)
		{
			/* eliminate this condition to match parts of input strings. */
			// if (!strcmp(t->output, s))
				match = 1;

			t = t->next;
		}
	}

	return match;
}

void
destroy_goto(struct gto *p)
{
	int i;

	assert(NULL != p);

	for (i = 0; i < p->ary_len; ++i)
		free(p->ary[i]);

	free(p->ary);

	if (NULL != p->accept)
	{
		int i;
		for (i = 0; i < p->accept_len; ++i)
		{
			if (p->accept[i].next)
			{
				struct output *n, *t;
				for (n = p->accept[i].next; n != NULL; n = t)
				{
					t = n->next;
					free(n->output);
					n->output = NULL;
					free(n);
					n = NULL;
				}
				p->accept[i].next = NULL;
			}
		}
		free(p->accept);
		p->accept = NULL;
	}
	if (NULL != p->failure) free(p->failure);
	if (NULL != p->delta[0]) free(p->delta[0]);
	if (NULL != p->delta) free(p->delta);

	free(p);
}

void
construct_delta(struct gto *g)
{
	struct queue *q;
	int i, a;

	g->delta = calloc(sizeof(int *), g->ary_len);

	g->delta[0] = calloc(sizeof(int), g->ary_len*128);

	for (i = 0; i < g->ary_len; ++i)
		g->delta[i] = g->delta[0] + i*128;

	q = queueinit();

	for (a = 0; a < 128; ++a)
	{
		g->delta[0][a] = g->ary[0][a];

		if (0 != g->ary[0][a])
			enqueue(q, g->ary[0][a]);
	}

	while (!queueempty(q))
	{
		int r = dequeue(q);

		for (a = 0; a < 128; ++a)
		{
			int s = g->ary[r][a];

			if (FAIL != s)
			{
				enqueue(q, s);
				g->delta[r][a] = s;
			} else
				g->delta[r][a] = g->delta[g->failure[r]][a];
		}
	}

	queuedestroy(q);
}

struct queue *
queueinit()
{
	struct queue *r;

	r = (struct queue *)malloc(sizeof(*r));
	assert(NULL != r);

	r->cbuf = (int *)malloc(CBUFSIZE*sizeof(int));
	assert(NULL != r->cbuf);

	r->in = 0;
	r->out = 0;
	r->ovfhead = r->ovftail = NULL;

	return r;
}

void
empty_error(struct queue *q)
{
	fprintf(stderr, "Tried to get off empty queue: in %d, out %d\n", q->in, q->out);
	exit(9);
}

void
enqueue(struct queue *q, int state)
{
	struct queue_node *newnode;

	assert(NULL != q);

	if (((q->in + 1)&CBUFMASK) != q->out)
	{
		assert(NULL != q->cbuf);
		q->cbuf[q->in] = state;
		q->in = (q->in+1)&CBUFMASK;

	} else {

		newnode = (struct queue_node *)malloc(sizeof *newnode);
		assert(NULL != newnode);
		newnode->state = state;
		newnode->next = NULL;

		if (NULL == q->ovftail)
		{
			q->ovfhead = q->ovftail = newnode;
		} else {
			q->ovftail->next = newnode;
			q->ovftail = newnode;
		}
	}
}

int
queueempty(struct queue *q)
{
	assert(NULL != q);
	return (q->in == q->out);
}

int
dequeue(struct queue *q)
{
	int r;

	assert(NULL != q);
	if (q->in == q->out) empty_error(q);

	assert(NULL != q->cbuf);
	r = q->cbuf[q->out];

	q->out = (q->out + 1)&CBUFMASK;

	if (NULL != q->ovfhead)
	{
		struct queue_node *tmp;

		q->cbuf[q->in]= q->ovfhead->state;
		q->in = (q->in+1)&CBUFMASK;
		tmp = q->ovfhead;
		q->ovfhead = q->ovfhead->next;
		if (NULL == q->ovfhead) q->ovftail = NULL;
		free(tmp);
	}

	return r;
}

void
queuedestroy(struct queue *q)
{
	assert(NULL != q);

	if (NULL != q->cbuf) free(q->cbuf);

	while (NULL != q->ovfhead)
	{
		struct queue_node *t = q->ovfhead->next;

		free(q->ovfhead);

		q->ovfhead = t;
	}

	free(q);
}
