CC = gcc
CFGLAGS = -Wall -g
UTILSOBJS = utils/confsocket.o

%.o: utils/%.c utils/%.h %.c %.h
	$(CC) $(CFGLAGS) -c $< -o $@

client: client.o $(UTILSOBJS)
	$(CC) $(CFGLAGS) $^ -o $@
	rm $^

server: server.o $(UTILSOBJS)
	$(CC) $(CFGLAGS) $^ -o $@
	rm $^

clean:
	rm -f *.o
