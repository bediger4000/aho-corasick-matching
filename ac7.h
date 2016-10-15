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

	int    state_counter;        /* Used during matching */
};

#define FAIL -1

struct gto *initialize_matching(int keywords_length, char *keywords[]);
int perform_match(struct gto *g, char *line);
void destroy_goto(struct gto *);
