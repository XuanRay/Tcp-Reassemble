CC := gcc -g

tr.out := ../TCP-Reassembler

objects := main.o tcp.o ip.o util.o hash.o file.o mydirent.o network.o

$(tr.out) : $(objects) libpcap.a
	$(CC) -o $(tr.out) $^

main.o : main.h tcp.h ip.h ./lib/mydirent.h ./lib/hash.h ./lib/util.h ./lib/file.h

tcp.o : tcp.h

ip.o : ip.h ./lib/util.h

util.o : ./lib/util.h ./lib/util.c
	$(CC) -o $@ -c ./lib/util.c

hash.o : ./lib/util.h ./lib/hash.h ./lib/hash.c
	$(CC) -o $@ -c ./lib/hash.c

file.o : ./lib/file.h ./lib/util.h ./lib/file.c
	$(CC) -o $@ -c ./lib/file.c

mydirent.o : ./lib/file.h ./lib/util.h ./lib/mydirent.c
	$(CC) -o $@ -c ./lib/mydirent.c

network.o : network.h


.PHONY : clean 
clean :
		-rm -f *.o