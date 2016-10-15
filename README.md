#Aho Corasick String Matching

A ibrary implementsion of the Aho-Corasick string matching algorithm.

This should be useful in cases where you'd like to find matches of
a large number of patterns in your code. `fgrep` would be nice, but
it's another executable. This code should fit perfectly.

Deliberately made easy to use and modify, at the expense of flexibility
and speed. It's only about 3 times slower than `fgrep` from the command
line, and you only have to:

1. Initialize with pattern string(s)
2. Call a function (repetively) on each line or chunk of input
3. Destroy the struct from (1).

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

I wrote this mostly before 1995, on a NeXT color slab. I've since incorporated it
in a few other projects. It forms the basis of the bracket abstraction pattern
matching in my Generalized Combinatory Logic interpreter. Given the date I wrote it,
it's pretty strict C89, and it probably doesn't have any endianess problems. It
certainly doesn't have any 32 vs 64 bit problems.

I wrote this directly from:

    String Matching: An Aid to Bibliographic Search 
    Alfred V Aho and Magaret J Corasick

It only matches ASCII keywords. It can handle UTF-8 input as
the string-to-be-matched (it just skips non-ASCII bytes), but not as the pattern keywords.

I ran it with GNU CC address sanitation and undefined behavior flags, and it passed
both without a peep. Running it under `valgrind` shows no memory leaks. GNU CC coverage
testing shows that almost every line except error handling code gets executed.
