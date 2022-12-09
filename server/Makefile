EXEC = server.o client.o server
all: ${EXEC}

client.o : client.c client.h
	gcc  -g -c  client.c

server.o : server.c server.h client.h
	gcc  -g -c  server.c

server : server.o client.o
	gcc server.o client.o -o server 

clean:
	rm -f server client *.o *~
