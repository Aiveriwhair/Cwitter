all: server

GCC_OPT= -Wall

server: server.o
	gcc -o server server.o

server.o: server.c
	gcc $(GCC_OPT) -c server.c


client: client.o
	gcc -o client client.o

client.o: client.c
	gcc $(GCC_OPT) -c client.c

clean:
	rm -f server client *.o *~