CC = gcc
CFLAGS = -O2 -I. -g -Wall -Wextra  #-fsanitize=address #-fsanitize=undefined

ac: ac_main.o ac7.o
	$(CC) $(CFLAGS) -o ac ac_main.o ac7.o


ac7.o: ac7.c ac7.h

ac_main.o: ac_main.c ac7.h

clean:
	-rm -rf *.o ac *core *.info *.gcda *.gcno covered

# Coverage testing using lcov
coverage:
	gcc -I. -fprofile-arcs -ftest-coverage -c ac_main.c
	gcc -I. -fprofile-arcs -ftest-coverage -c ac7.c
	gcc -I. -fprofile-arcs -ftest-coverage -o ac ac_main.o ac7.o
	./ac aard vark /usr/share/dict/words < /usr/share/dict/words > /dev/null
	./ac his hers theirs mine yalls zymurgy /usr/share/dict/words < /usr/share/dict/words > /dev/null
	./ac "AA" "AAA" "Aachen" "aah" "Aaliyah" "Aaliyah's" "aardvark" "aardvark's" "aardvarks" "Aaron" "AA's" "AB" "ab" "ABA" "aback" "abacus" "abacuses" "abacus's" < /usr/share/dict/words > /dev/null
	./ac abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ '~`!@#$%^&*()_-+={[}]|:;"<,>.?/abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789' < /usr/share/dict/words > /dev/null
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info \
		--output-directory covered --title "Aho-Corasick Matching" \
		--show-details --legend
