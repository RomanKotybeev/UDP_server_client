# A UDP server and client
An example of how a UDP server and client can be used with sockets.
You can create as many clients as you want. Server responses how many clients
are connected with him and gives the length of the client message.

## Build
Use Makefile to build a project.  I use the GCC compiler, but you can change
it. A server and client compile separately. To build a UDP server use command

```bash
make server
```
To build a UDP client:

```bash
make client
```

## Use
Server requires two arguments: address of your server and port. Use any port
you like:

```bash
./server 172.16.1.101 6666
```

Server runs as a daemon (in a background).

Client requires four arguments: server address, client address, port and
message. For example,

```bash
./client 172.16.1.101 192.168.0.111 6666 "Hello, World"
```

The result:

```bash
# of clients = 1, # of bytes recieved = 12
```
