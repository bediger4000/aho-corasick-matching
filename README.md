#Aho Corasick String Matching

Library implementsion

#Example Use

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


#Example Program

Running `make` should yield an executable named `ac`. `ac` accepts keywors to look for
as individual parameters on the command line, and reads `stdin` and looks for a match
each input line. Use it like this:

    $ ./ac his hers theirs mine yalls zymurgy < /usr/share/dict/words
    Amherst
    amphetamine
    amphetamine's
    amphetamines
    anarchism
    anarchism's
      ...
    withers
    withers's
    zithers
    zymurgy
    zymurgy's
    $

Invoking `./runtests` should put the example program `ac` through paces compared to `fgrep`.

Invoking `make coverage` builds a code-coverage (gcov) executable, runs some test
cases with it, and then invokes `lcov` and `genhtml` to give you the directory `covered/`
so you can see what the test cases actually use in the code.

#Implementation

I wrote this directly from:

    String Matching: An Aid to Bibliographic Search 
    Alfred V Aho and Magaret J Corasick

It only matches ASCII keywords. It can handle UTF-8 input as
the string-to-be-matched (it just skips non-ASCII bytes), but not as the pattern keywords.
