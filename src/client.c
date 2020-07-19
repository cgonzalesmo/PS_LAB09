#include <sys/types.h>          /* some systems still require this */
#include <sys/stat.h>
#include <stdio.h>              /* for convenience */
#include <stdlib.h>             /* for convenience */
#include <stddef.h>             /* for offsetof */
#include <string.h>             /* for convenience */
#include <unistd.h>             /* for convenience */
#include <signal.h>             /* for SIG_ERR */ 
#include <netdb.h> 
#include <errno.h> 
#include <syslog.h> 
#include <sys/socket.h> 
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <fcntl.h>

#define BUFLEN 100 // 10 Mb

int main(int argc, char ** argv)
{
  int client;

  if(argc != 5){
  	printf("Error!!\n");
    
    return -1;
  }

  char *ipadd = argv[1];
  int puerto = atoi(argv[2]);
  char *add = argv[3];
  char *nombre = argv[4];
  

    struct sockaddr_in direccion_cliente;
    memset(&direccion_cliente, 0, sizeof(direccion_cliente));
  direccion_cliente.sin_family = AF_INET;		
  direccion_cliente.sin_port = htons(puerto);	
  direccion_cliente.sin_addr.s_addr = inet_addr(ipadd);

  client = socket(((struct sockaddr *)&direccion_cliente)->sa_family, SOCK_STREAM, 0);
  if (client == -1)
  {
  	printf("Error al abrir el socket\n");
  	return -1;
  }
  printf("Abierto el socket para el cliente...\n");

  //Conectamos
  int conectar = connect(client, (struct sockaddr *)&direccion_cliente, sizeof(direccion_cliente));
  if (conectar != 0)
  {
  	printf("Error : No es posible conectar\n");
  	return 1;
  }
  printf("conectado...\n");
  
  //Enviamos la ruta del archivo para que el servidor lo busque
  send(client, add, strlen(add), 0);
  
  //Leemos la respuesta del servidor
  char * buf = malloc(1);

  int fd = open(nombre,O_CREAT|O_TRUNC|O_RDWR,S_IRWXU);
  if(fd == -1){
  	printf("Error al crear el archivo..\n");
  	return -1;
  }

  int n=1;
  while(n>0)
  {
    n = recv(client, buf, n, 0);
    write(fd,buf,n);
    memset(buf, 0, 1);
  }
  close(fd);
  return 0;
}
