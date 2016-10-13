#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <ac7.h>
#include <cb.h>

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
