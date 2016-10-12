CC = gcc
CFLAGS = -I. -g -Wall -Wextra #-fsanitize=undefined -fsanitize=address

ac: ac_main.o ac7.o cb.o
	$(CC) $(CFLAGS) -o ac ac_main.o ac7.o cb.o

cb.o: cb.c cb.h

ac7.o: ac7.c ac7.h cb.h

ac_main.o: ac_main.c ac7.h

clean:
	-rm -rf *.o ac *core
