.PHONY: all

all: server client server2

server: ./src/servidor_multiproceso.c
	gcc -Wall ./src/servidor_multiproceso.c -o ./bin/servidor

server2: ./src/server.c
	gcc -Wall ./src/server.c -o ./bin/server

client: ./src/client.c
	gcc -Wall ./src/client.c -o ./bin/cliente

clean:
	rm -r ./files  ./bin/*