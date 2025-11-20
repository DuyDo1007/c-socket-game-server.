CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -pthread

all: server/server client/client

server/server: server/server.o server/handler.o server/net.o
	$(CC) $(CFLAGS) -o server/server server/server.o server/handler.o server/net.o $(LDFLAGS)

server/server.o: server/server.c server/handler.h server/net.h
	$(CC) $(CFLAGS) -Iserver -c server/server.c -o server/server.o

server/handler.o: server/handler.c server/handler.h server/net.h
	$(CC) $(CFLAGS) -Iserver -c server/handler.c -o server/handler.o

server/net.o: server/net.c server/net.h
	$(CC) $(CFLAGS) -Iserver -c server/net.c -o server/net.o


client/client: client/client.o client/net.o
	$(CC) $(CFLAGS) -o client/client client/client.o client/net.o

client/client.o: client/client.c client/net.h
	$(CC) $(CFLAGS) -Iclient -c client/client.c -o client/client.o

client/net.o: client/net.c client/net.h
	$(CC) $(CFLAGS) -Iclient -c client/net.c -o client/net.o

	
clean:
	rm -f server/server server/*.o client/client client/*.o