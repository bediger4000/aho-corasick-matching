CC = gcc
CFLAGS = -I. -g -Wall -Wextra #-fsanitize=undefined -fsanitize=address

ac: ac_main.o ac7.o
	$(CC) $(CFLAGS) -o ac ac_main.o ac7.o


ac7.o: ac7.c ac7.h

ac_main.o: ac_main.c ac7.h

clean:
	-rm -rf *.o ac *core
