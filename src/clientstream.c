#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "shell_commands.h"

// the port client will be connecting to
#define PORT 3490
// max number of bytes we can get at once
#define MAXDATASIZE 5000

#define LINE_MAX 30

int main(int argc, char *argv[])
{
  int sockfd, numbytes;
  struct hostent *he;

  //Respuesta del servidor
  char server_usr[MAXDATASIZE];
  char server_execution[MAXDATASIZE];
  
  //shell variables
  char command[LINE_MAX]; // podemos usarlo por el fgets
  char *username = (char *) malloc(MAXDATASIZE);
  //char *result = (char *) malloc(MAXDATASIZE);

  // connector’s address information
  struct sockaddr_in their_addr;

  // if no command line argument supplied
  if(argc != 2)
  {
    fprintf(stderr, "Client-Usage: %s hostname_del_servidor\n", argv[0]);
    // just exit
    exit(1);
  }

  // get the host info
  if((he=gethostbyname(argv[1])) == NULL)
  {
    perror("gethostbyname()");
    exit(1);
  }
  else
    printf("Client-The remote host is: %s\n", argv[1]);

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket()");
    exit(1);
  }
  else
    printf("Client-The socket() sockfd is OK...\n");


  // host byte order
  their_addr.sin_family = AF_INET;
  // short, network byte order
  printf("Server-Using %s and port %d...\n", argv[1], PORT);
  their_addr.sin_port = htons(PORT);
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  // zero the rest of the struct
  memset(&(their_addr.sin_zero), '\0', 8);

  if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("connect()");
    exit(1);
  }
  else
    printf("Client-The connect() is OK...\n");

  //---------------INICIO AQUI------------------------
  
  // Se va a recibir el username del servidor
  if ((numbytes = recv(sockfd, server_usr, MAXDATASIZE-1, 0)) == -1) {
    perror("recv() fail: No se recibio username");
    exit(1);
  }
  else {
    //Usuario obtenido
    server_usr[numbytes] = '\0';
    username = (char *)realloc(username, strlen(server_usr) * sizeof(char));
    strcpy(username, server_usr);

  }

  printf("%s\n", server_usr);

  while(1) {
    
    //Se lee comando del usuario
    read_command(username, command);

    if (strcmp(command, "exit") == 0) 
      break;
      
    if (send(sockfd, command, strlen(command), 0) == -1)
      perror("Server-send(): No se mando comando");

    //Se recibe respuesta del servidor   
    if ((numbytes = recv(sockfd, server_execution, MAXDATASIZE-1, 0)) == -1) {
      perror("recv() fail: No se recibio respuesta de la ejecucion");
      exit(1);
    }
    else {
      //Respuesta de execvp obtenida
      server_execution[numbytes] = '\0';


      //Manejo de respuesta de execvp recibida
      //result = (char *)realloc(result, strlen(server_execution) * sizeof(char));
      //strcpy(result, server_execution);
      //printf("%s\n", result);

      printf("%s\n", server_execution);
      fflush(stdout);
      server_execution[0] = '\0';

    }

  }

  /*
  printf("Escribe un mensaje a enviar\n");
  char command[LINE_MAX]; // podemos usarlo por el fgets
  fgets(command,LINE_MAX,stdin);
  printf("El mensaje a enviar es: %s", command);
  // Envia el mensaje al servidor
  if (send(sockfd, command, strlen(command), 0) == -1)
    perror("Server-send() error lol!");

  // El cliente ya escribio, ahora va a leer la respuesta del servidor
  if((numbytes = recv(sockfd, response, MAXDATASIZE-1, 0)) == -1)
  {
    perror("recv()");
    exit(1);
  }
  else
    printf("Client-The recv() is OK...\n");

  response[numbytes] = '\0';
  printf("Client-Received: %s", response);
  
  */
  
  printf("Client-Closing sockfd\n");
  close(sockfd);
  return 0;
}

