all: ssu_find-md5 ssu_find-sha1 ssu_help ssu_sdup
CC=gcc

ssu_find-md5: ssu_find-md5.o ssu_command.o
	$(CC) -o $@ $^ -lcrypto

ssu_find-sha1: ssu_find-sha1.o ssu_command.o
	$(CC) -o $@ $^ -lcrypto

ssu_help: ssu_help.o
	$(CC) -o $@ $^

ssu_sdup: ssu_sdup.o ssu_command.o
	$(CC) -o $@ $^

ssu_find-md5.o: ssu_find-md5.c
	$(CC) -c $^ -lcrypto

ssu_find-sha1.o: ssu_find-sha1.c
	$(CC) -c $^ -lcrypto

ssu_help.o: ssu_help.c
	$(CC) -c $^

ssu_sdup.o: ssu_sdup.c
	$(CC) -c $^ 

ssu_command.o: ssu_command.c
	$(CC) -c $^

clean :
	rm ssu_find-md5.o
	rm ssu_find-md5
	rm ssu_find-sha1.o
	rm ssu_find-sha1
	rm ssu_help.o
	rm ssu_help
	rm ssu_sdup.o
	rm ssu_sdup
	rm ssu_command.o
