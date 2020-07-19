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

#define BUFLEN 100
#define BUFRD 100



int main(int argc, char ** argv)
{
  int sd;
  
  if(argc != 3)
  {
  	printf("Error!!\n");
    printf("Modo de Uso: ./servidor <ip> <numero de puerto>\n");
    return -1;
  }

  char* ipadd = argv[1];
  int puerto = atoi(argv[2]);

  struct sockaddr_in direccion_servidor;
  memset(&direccion_servidor, 0, sizeof(direccion_servidor));
  direccion_servidor.sin_family = AF_INET;
  direccion_servidor.sin_port = htons(puerto);
  direccion_servidor.sin_addr.s_addr = inet_addr(ipadd);

 
  sd = socket(((struct sockaddr *)&direccion_servidor)->sa_family, SOCK_STREAM, 0);
  if (sd == -1)
  {
  	printf("Error -1 en socket\n");
  	return -1;
  }
//mantener open
  int keep = 1;
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &keep, sizeof(keep));


  int link = bind(sd, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor));
  if(link != 0)
  {
  	printf("Error!!!\n");
  	printf("No se puede enlazar al puerto : dirección ya está en uso\n");
    return -1;
  }

  
  int escuchar = listen(sd,100);
  if(escuchar == -1)
  {
  	printf("No es posible escuchar en ese puerto\n");
  	return -1;
  }
  printf("Servidor Iniciado.\n");

  struct sockaddr_in direccion_cliente;
  memset(&direccion_servidor, 0, sizeof(direccion_cliente));
  unsigned int tam = sizeof(direccion_cliente);

  
  
  while(1)
  {
    int client = accept(sd,(struct sockaddr *)&direccion_cliente,&tam);
//FORK
    int pid = fork();

    if (pid==0){
      sigset_t set;
      sigemptyset(&set);
      sigaddset(&set,SIGTSTP);
      sigprocmask(SIG_BLOCK, &set, 0);

      char *add = (char *)malloc(BUFLEN*sizeof(char *));
      char *file = (char *)malloc(BUFRD*sizeof(char *));
      
      recv(client, add, BUFLEN, 0);
      printf("\nUsuario conectado\n");

      printf("Buscando archivo: %s\n", add);

      int fd = open(add, O_RDONLY,S_IROTH);
      
      if (fd < 0)
      {
        printf("Error al abrir el archivo\n");
        char * mensaje = "Error al abrir el archivo";
        send(client, mensaje, strlen(mensaje) ,0);
        close(client);
        continue;
      }
        
      printf("Archivo abierto\n");
      int filesize;
        
      while((filesize = read(fd, file, BUFRD)) > 0)
      {
        send(client, file, filesize, 0);
        memset(file, 0, BUFRD);
      }
        
      printf("Archivo enviado\n");
      close(fd);
      free(file);
      close(client);
      break;
    }
    else{
      close(client);
      continue;
    }

  }

  return 0;
}