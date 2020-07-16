#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

/*Definición de constantes*/
#define BUFFSIZE 1
#define ERROR -1

#ifndef HOST_NAME_MAX 
#define HOST_NAME_MAX 256 
#endif
int comprobador=1;

void catch(int sign);

int main(int argc, char *argv[]){
	struct sockaddr_in stSockAddr;
    struct sockaddr_in clSockAddr;
	FILE *archivo;
	char *host;
	int SocketServerFD;
	int SocketClientFD;
	char mensaje[80];
	int clientLen;
	int puerto;
	pid_t pid;
	sigset_t senales,old;

	/*Verifica que el número de parametros sea el correcto*/
	if(argc == 1){
		printf("Uso: ./servidor <ip> <puerto>\n");
		exit(-1);
	}

	if(argc != 3){
		printf( "por favor especificar un numero de puerto\n");
	}

	/*Se crea el socket*/
	if((SocketServerFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == ERROR){
		perror("No se puede crear el socket");
		exit(EXIT_FAILURE);
	}//End if-SocketFD
 
	/*Se configura la dirección del socket*/
	memset(&stSockAddr, 0, sizeof stSockAddr);
	puerto = atoi(argv[2]);
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(puerto);
	stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
	if(bind(SocketServerFD,(struct sockaddr *)&stSockAddr, sizeof stSockAddr) == ERROR){
		perror("Error en bind");
		close(SocketServerFD);
		exit(EXIT_FAILURE);
	}//End if-bind
	inet_pton(AF_INET, argv[1], &stSockAddr.sin_addr);
	printf("Socket atado a la dirección %s\n",(char *)inet_ntoa(stSockAddr.sin_addr));
	
	//darle el nombre del puerto que esta escuchando
	if ((host = malloc(HOST_NAME_MAX)) == NULL) 
		printf(" malloc error"); 
	gethostname( host, HOST_NAME_MAX);	
	printf("Nombre del host: %s\n",host);

	if(listen(SocketServerFD, 10) == ERROR){
		perror("Error listen");
		close(SocketServerFD);
		exit(EXIT_FAILURE);
	}//End if-listen

	while (comprobador){
		//Creamos los SUBPROCESOS
		if (( pid = fork()) < 0) {
			perror(" fork error");
		} else if (pid == 0) { /* child */
			sigfillset(&senales);
			sigdelset(&senales,SIGTSTP);
			sigprocmask(SIG_UNBLOCK,&senales,&old);
		} else {
				//Atrapamos la señal
				signal(SIGTSTP,catch);
		} 


		clientLen = sizeof(clSockAddr);

		//Espera por la conexión de un cliente//
		SocketClientFD = accept(SocketServerFD, 
						    (struct sockaddr *) &clSockAddr,(socklen_t *)
						    &clientLen);
		if (SocketClientFD < 0){
			perror("Fallo para aceptar la conexión del cliente");
		}//End if-accept

		/*Se configura la dirección del cliente*/
		clSockAddr.sin_family = AF_INET;
		clSockAddr.sin_port = htons(puerto);
		printf("Cliente conectado: %s\n",inet_ntoa(clSockAddr.sin_addr));

		/*Se recibe la ruta a buscar del cliente*/
        char rutacliente[BUFFSIZE]={0};
        char imagen[BUFFSIZE]={0};	
        read(SocketClientFD,rutacliente,sizeof(rutacliente));
		//Termina la recepción de la ruta
        archivo=fopen(rutacliente,"rb");
        while(!feof(archivo)){
            fread(imagen,sizeof(char),BUFFSIZE,archivo);
            if(send(SocketClientFD,imagen,BUFFSIZE,0) == ERROR)
                perror("Error al enviar el arvhivo:");
	    }
		
		read(SocketClientFD,mensaje,sizeof(mensaje));
		printf("\nConfirmación enviada:\n%s\n",mensaje);
		
	}//End infinity while

 	close(SocketClientFD);
	close(SocketServerFD);
	return 0;
}//End main program

void catch(int sign)
{
	printf("Señal: %d atrapada!\n",sign);
	comprobador=-1;
}
