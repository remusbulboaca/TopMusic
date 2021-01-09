/* cliTCPIt.c - Exemplu de client TCP
   Trimite un numar la server; primeste de la server numarul incrementat.
         
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main(int argc, char *argv[])
{
  int sd;                    // descriptorul de socket
  struct sockaddr_in server; // structura folosita pentru conectare
      // mesajul trimis
  int nr = 0;
  char buf[50];
  char cmdin[1024]; // raspunsul dat de server
  char cmdout[1024]; // comanda trimisa de client

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
  {
    printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
    return -1;
  }

  /* stabilim portul */
  port = atoi(argv[2]);

  /* cream socketul */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Eroare la socket().\n");
    return errno;
  }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons(port);

  /* ne conectam la server */
  if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[client]Eroare la connect().\n");
    return errno;
  }
  
  while (1)
  {
    /* citirea mesajului */
    printf("[client]Introduceti o comanda: ");
    fflush(stdout);

    memset(cmdout,0,sizeof(cmdout));
    read(0, cmdout,sizeof(cmdout));
    //scanf("%d",&nr);

    printf("[client] Am citit %s\n", cmdout);
    
    /* trimiterea mesajului la server */
    /*TRIMITEM COMANDA CITITA INITIAL LA SERVER*/
    if (write(sd, &cmdout, sizeof(cmdout)) <= 0)
    {
      perror("[client]Eroare la write() spre server.\n");
      return errno;
    }
    /*Asteapm raspunsul si il validam*/
    memset(cmdin,0,sizeof(cmdin));
    if (read(sd, &cmdin, sizeof(cmdin)) < 0)
    {
      perror("[client]Eroare la read() de la server.\n");
      return errno;
    }
    printf("[Server] Comanda primita: %s\n", cmdin);
    
    if(strcmp(cmdin,"Bye...") == 0){
      exit(0);
    }
    
    }
  }
