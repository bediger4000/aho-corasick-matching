#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ac7.h>

int
main(int ac, char **av)
{
	struct gto *g;
	char bp[BUFSIZ];

	if (1 == ac)
		return 1;

	g = initialize_matching(ac - 1, &av[1]);

	/* do the matching */
	while (NULL != (fgets(bp, sizeof(bp), stdin)))
	{
		bp[strlen(bp) - 1] = '\0';
		if (perform_match(g, bp))
			printf("matched \"%s\"\n", bp);
	}

	destroy_goto(g);

	return 0;
}
