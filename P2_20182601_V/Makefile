OBJECTS=ssu_sdup.o ssu_find_md5.o ssu_find_sha1.o ssu_help.o
TARGET=my_file
CC=gcc

$(TARGET) : $(OBJECTS)
	$(CC) -o $@ $^

ssu_sdup.o: ssu_sdup.c
	$(CC) -c $^

ssu_find_md5.o: ssu_find_md5.c
	$(CC) -c $^ -lcrypto

ssu_find_sha1.o: ssu_find_sha1.c
	$(CC) -c $^ -lcrypto

ssu_help.o: ssu_help.c
	$(CC) -c $^

clean :
	rm ssu_sdup.o
	rm ssu_find_md5.o
	rm ssu_find_sha1.o
	rm ssu_help.o
