CC=gcc

ssu_sfinder: ssu_sfinder.o
	$(CC) -o $@ $^ -lcrypto

ssu_sfinder.o: ssu_sfinder.c
	$(CC) -c $^ -lcrypto

clean :
	rm ssu_sfinder.o
	rm ssu_sfinder
