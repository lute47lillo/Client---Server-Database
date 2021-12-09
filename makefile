CFLAGS := -Wall -ansi -pedantic -g -c

ALL: client.exe server.exe

util.o : util.c
	gcc $(CFLAGS) util.c

client.o : client.c
	gcc $(CFLAGS) client.c

client.exe : client.o util.o
	gcc -g -o client.exe client.o util.o

server.o : server.c
	gcc $(CFLAGS) server.c

server.exe : server.o
	gcc -g -o server.exe server.o util.o

clean:
	rm -f *.o
	rm -f *.exe
