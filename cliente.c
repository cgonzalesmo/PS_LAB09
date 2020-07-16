#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFSIZE 1
#define	ERROR	-1
//#define	PUERTO	4555

/*Prototipos de función*/
void recibirArchivo(int SocketFD, FILE *file, char name);
void enviarConfirmacion(int SocketFD);
void enviarMD5SUM(int SocketFD);

int main(int argc, char *argv[]){
	struct sockaddr_in stSockAddr;
	int Res;
	int SocketFD;
	char buffer[BUFFSIZE];
	int puerto;
	FILE *archivo;

	if(argc == 1){
		printf("Uso: ./cliente <ip> <puerto> <directorio a leer> <nombre del archivo>\n");
		exit(-1);
	}

	if(argc != 5){
		printf( "por favor especificar un numero de puerto\n");
	}

	/*Se crea el socket*/
	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*Se verifica la integridad del archivo*/
	if(!archivo){
		perror("Error al abrir el archivo:");
		exit(EXIT_FAILURE);
	}
    
	/*Se verifica la integridad del socket*/
	if (SocketFD == ERROR){
		perror("cannot create socket");
		exit(EXIT_FAILURE);
	}

	/*Se configura la dirección del socket del cliente*/
	memset(&stSockAddr, 0, sizeof stSockAddr);
	puerto = atoi(argv[2]);
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(puerto);
	Res = inet_pton(AF_INET, argv[1], &stSockAddr.sin_addr);

	if (0 > Res){
		perror("error: El primer parametro no es una familia de direcciónes");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}else if (Res == 0){
		perror("char string (El segundo parametro no contiene una dirección IP válida");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	if (connect(SocketFD, (struct sockaddr *)&stSockAddr, sizeof stSockAddr) == ERROR){
		perror("Error a la hora de conectarse con el cliente");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	printf("Se ha conectado con el servidor:%s\n",(char *)inet_ntoa(stSockAddr.sin_addr));

	char ruta[BUFFSIZE]={0};
	if(send(SocketFD,ruta,BUFFSIZE,0) == ERROR)
			perror("Error al enviar el ruta:");

	// se recibe el archivo
	recibirArchivo(SocketFD,archivo,argv[4]);
	
	fclose(archivo);
	close(SocketFD);

	return 0;
}//End main

void recibirArchivo(int SocketFD, FILE *file,char name){
	char buffer[BUFFSIZE];
	int recibido = -1;

	/*Se abre el archivo para escritura*/
	file = fopen(name,"wb");
	while((recibido = recv(SocketFD, buffer, BUFFSIZE, 0)) > 0){
		//printf("%s",buffer);
		fwrite(buffer,sizeof(char),1,file);
	}//Termina la recepción del archivo
	enviarConfirmacion(SocketFD);
	fclose(file);
	

}//End recibirArchivo procedure

void enviarConfirmacion(int SocketFD){
	char mensaje[80] = "Paquete Recibido";
	printf("\nConfirmación enviada\n");
	if(write(SocketFD,mensaje,sizeof(mensaje)) == ERROR)
			perror("Error al enviar la confirmación:");
}//End enviarConfirmacion
