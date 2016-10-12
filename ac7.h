struct output {
	struct output *next;
	char *output;
};

struct gto {
	int  **ary;                  /* transition table */
	int    ary_len;              /* max state currently in table */

	int   *failure;              /* failure states */
	int  **delta;

	struct output *accept;       /* stack of acceptance output */
	int    accept_len;           /* max state for accept states */
};

#define FAIL -1

void add_state(struct gto *p, int state, char input, int new_state);
void set_accept(struct gto *p, int state, char *output);
void construct_goto(char *keywords[], int k, struct gto *g);
void construct_failure(struct gto *g);
void construct_delta(struct gto *g);
struct gto *init_goto(void);
void        destroy_goto(struct gto *);

int perform_match(struct gto *g, char *line);
