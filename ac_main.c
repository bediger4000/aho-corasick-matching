#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ac7.h>

int
main(int ac, char **av)
{
	int i, k = 0;
	char **keywords;
	struct gto *g;
	char bp[BUFSIZ];

	keywords = (char **)malloc(sizeof(char *) * (ac - 1));

	for (i = 1; i < ac; ++i)
	{
		keywords[i - 1] = malloc(strlen(av[i]) + 1);

		strcpy(keywords[i-1], av[i]);
		++k;
	}

	g = init_goto();

	construct_goto(keywords, k, g);
	construct_failure(g);
	construct_delta(g);

	/* do the matching */
	while (NULL != (fgets(bp, sizeof(bp), stdin)))
	{
		bp[strlen(bp) - 1] = '\0';
		if (perform_match(g, bp))
			printf("matched \"%s\"\n", bp);
	}

	destroy_goto(g);

	for (i = 0; i < ac - 1; ++i)
		free(keywords[i]);

	free(keywords);

	return 0;
}
