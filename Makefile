all: Servidor_Multiproceso Cliente

Servidor_Multiproceso: servidor_multiproceso.c
	gcc -Wall -g $^ -o Servidor_Multiproceso

Cliente: cliente.c
	gcc -Wall $^ -o Cliente

clean:
	rm -rf Cliente Servidor_Multiproceso verificacion archivoRecibido
