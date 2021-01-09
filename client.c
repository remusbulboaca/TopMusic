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
  char cmdin[8888]; // raspunsul dat de server
  char cmdout[8888]; // comanda trimisa de client
    char login_user[50];
    char login_password[50];
    /*Variablie reciclabile*/
    char auxvar1[100];
    char auxvar2[100];
    char auxvar3[100];
    char auxvar4[100];
    char auxvar5[100];

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
      cmdout[strlen(cmdout)-1] = 0; //scapam de newline

    printf("[client] Ai introdus: %s\n", cmdout);
      if (strcmp(cmdout,"login")==0) {
          system("clear");
          printf("Ai introdus comanda login!\n");
          printf("Introdu usernameul:\n");
          printf(">>");
          memset(login_user,0,sizeof(login_user));
          scanf("%s",login_user);
          printf("Introdu parola:\n");
          printf(">>");
          memset(login_password,0,sizeof(login_password));
          scanf("%s",login_password);
          strcat(cmdout,"##");
          strcat(cmdout,login_user);
          strcat(cmdout,"##");
          strcat(cmdout,login_password);
          printf("Comanda de trimis: %s \n",cmdout);
      }
      
      if (strcmp(cmdout,"add_song")==0) {
          system("clear");
          printf("ADAUGA O PIESA:\n");
          printf("Introdu titlul:\n");
          printf(">> ");
          memset(auxvar1,0,sizeof(auxvar1));
          scanf("%s",auxvar1);
          printf("Introdu o descriere:\n");
          printf(">> ");
          memset(auxvar2,0,sizeof(auxvar2));
          scanf("%s",auxvar2);
          printf("Introdu genul muzical:\n>> ");
          memset(auxvar3,0,sizeof(auxvar3));
          scanf("%s",auxvar3);
          printf("Introdu un link:\n>> ");
          memset(auxvar4,0,sizeof(auxvar4));
          scanf("%s",auxvar4);
          strcat(cmdout,"##");
          strcat(cmdout,auxvar1);
          strcat(cmdout,"##");
          strcat(cmdout,auxvar2);
          strcat(cmdout,"##");
          strcat(cmdout,auxvar3);
          strcat(cmdout,"##");
          strcat(cmdout,auxvar4);
      }
      
      if (strcmp(cmdout,"register")==0) {
          system("clear");
          printf("REGISTER\n");
          printf("Introdu un username:\n");
          printf(">> ");
          memset(login_user,0,sizeof(login_user));
          scanf("%s",login_user);
          printf("Introdu parola:\n");
          printf(">> ");
          memset(login_password,0,sizeof(login_password));
          scanf("%s",login_password);
          strcat(cmdout,"##");
          strcat(cmdout,login_user);
          strcat(cmdout,"##");
          strcat(cmdout,login_password);
          printf("Comanda de trimis: %s \n",cmdout);
      }
    
    /* trimiterea mesajului la server */
    /*TRIMITEM COMANDA CITITA INITIAL LA SERVER*/
    if (write(sd, &cmdout, sizeof(cmdout)) <= 0)
    {
      perror("[client]Eroare la write() spre server.\n");
      return errno;
    }
      
    /*Asteapm raspunsul si il afisam*/
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
