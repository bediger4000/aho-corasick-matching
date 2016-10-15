#include <stdio.h>
#include <string.h>
#include <ac7.h>
int main(int ac, char **av)
{
	struct gto *g;
	char buf[BUFSIZ];

	if (1 == ac) return 1;

	g = initialize_matching(ac - 1, &av[1]);

	while (fgets(buf, sizeof(buf), stdin))
	{
		buf[strlen(buf) - 1] = '\0';
		if (perform_match(g, buf))
			printf("%s\n", buf);
	}

	destroy_goto(g);

	return 0;
}
