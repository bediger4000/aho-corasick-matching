#include <sfio.h>
#include <stdlib.h>
#include <libc.h>
#include <assert.h>

#include <ac7.h>

int
main(int ac, char **av)
{
	int i, k = 0;
	char **keywords;
	struct gto *g;
	char *bp;

	keywords = (char **)malloc(sizeof(char *) * (ac - 1));
	assert(NULL != keywords);

	for (i = 1; i < ac; ++i)
	{
		keywords[i - 1] = malloc(strlen(av[i]) + 1);
		assert(NULL != keywords[i - 1]);

		strcpy(keywords[i-1], av[i]);
		++k;
	}

	g = init_goto();

	construct_goto(keywords, k, g);
	construct_failure(g);
	construct_delta(g);

	for (i = 0; i < g->accept_len; ++i)
		sfprintf(sfstdout, "accept[%d] is %d\n", i, g->accept[i]);

	/* do the matching */
	while (NULL != (bp = sfgetr(sfstdin, '\n', 1)))
		if (perform_match(g, bp))
			sfprintf(sfstdout, "matched on \"%s\"\n", bp);

	destroy_goto(g);

	for (i = 0; i < ac - 1; ++i)
		free(keywords[i]);

	free(keywords);

	return 0;
}
