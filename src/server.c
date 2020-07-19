#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h> // exit()
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>

void error(char *msg)
{
    perror(msg);
    exit(-1);
}


void dostuff(int sockfd);

int main(int argc, char *argv[])
{
  // definicion de las variables
  int sockfd, client_socket;
  struct sockaddr_in serv_addr;

  // argumentos de entrada
  if (argc < 2) {
    fprintf(stderr,"ERROR, la direccion no esta ingresada\n");
    exit(-1);
  }
  if (argc < 3) {
    fprintf(stderr,"ERROR, el puerto no esta especificado\n");
    exit(-1);
  }

  // inicializacion del socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error("ERROR al iniciar el socket");
  }

  // set el puerto y los datos necesarios para inicializar el server
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port = htons(atoi(argv[2]));
  if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
    error("ERROR en bind socket");
  }
  // FIN inicializar socket
  listen(sockfd,100000);
  
  while(1) {
    client_socket = accept(sockfd, NULL, NULL);
    if (client_socket < 0) {
      close(client_socket);
      printf("Erro al conectarse \n");
    }
    dostuff(client_socket);
  }

  return 0;
}


void dostuff(int newsockfd){

  char buffNombreArchivo[256];
  unsigned char buffInfo[8000000]={0};
  int n;

  bzero(buffNombreArchivo,sizeof(buffNombreArchivo));
  bzero(buffInfo,sizeof(buffInfo));

  n = read(newsockfd, buffNombreArchivo, sizeof(buffNombreArchivo));

  if (n < 0) 
    error("ERROR al leer el socket");

  //Se abre archivo que contiene información solicitada por el cliente
  FILE *fp = fopen(buffNombreArchivo,"rb");

  if(fp==NULL) {
    printf("El archivo no existe\n");
    close(newsockfd);

  } else {
    
    int nread;
    //Se divide la información y se le envía el cliente en "pedazos"
    while((nread = fread(buffInfo,1,sizeof(buffInfo),fp))>0){
      write(newsockfd,buffInfo,nread);
      bzero(buffInfo,sizeof(buffInfo));

      if (feof(fp)){
        printf("Terminado de copiar el archivo. Comando ejecutado: get %s\n",buffNombreArchivo);
        write(newsockfd,"",0);
      }
    }
    fclose(fp);
    close(newsockfd);

    printf("------------------------------------------------------------\n");

  }

}