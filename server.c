/* servTCPConcTh2.c - Exemplu de server TCP concurent care deserveste clientii
   prin crearea unui thread pentru fiecare client.
   Asteapta un numar de la clienti si intoarce clientilor numarul incrementat.
	Intoarce corect identificatorul din program al thread-ului.
  
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sqlite3.h>

/* portul folosit */
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */

sqlite3 * db;
extern int errno;

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

static int callback (void *str, int argc, char **argv, char **azColName)
{
    int i;
    char* data = (char*) str;

    for (i = 0; i < argc; i++)
    {
        strcat (data, azColName[i]);
        strcat (data, " = ");
        if (argv[i])
            strcat (data, argv[i]);
        else
            strcat (data, "NULL");
        strcat (data, "\n");
    }

    strcat (data, "\n");
    return 0;
}

int main ()
{
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int nr;		//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	int i=0;
  

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }
  /* servim in mod concurent clientii...folosind thread-uri */
  while (1)
    {
      int client;
      thData * td; //parametru functia executata de thread     
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      // client= malloc(sizeof(int));
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
	
        /* s-a realizat conexiunea, se astepta mesajul */
    
	// int idThread; //id-ul threadului
	// int cl; //descriptorul intors de accept

	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->cl=client;

	pthread_create(&th[i], NULL, &treat, td);	      
				
	}//while    
};				
static void *treat(void * arg)
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);	
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
		fflush (stdout);		 
		pthread_detach(pthread_self());		
		raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
  		
};


void raspunde(void *arg)
{
  char cmdin[8888]; //comanda primita de la client
  char cmdout[8888]; // raspunsul trimis clientului
    char logincmd[50];
    char registercmd[50];
    char username[50];
    char password[50];
	struct thData tdL;
    int is_user = 0;
    int is_admin = 0;
    char sql[8888];
    char str[8888];
    char* zErrMsg;
    
    
	tdL= *((struct thData*)arg);
  
  sqlite3 *db;
  sqlite3_stmt *res;
    
    /*Initializare DB*/
  int rc = sqlite3_open("TopMusic.db", &db);
    
  if (rc != SQLITE_OK) {
      fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
    }
  
  
  while (1) 
  {
    /*   Citim comanda   */
    fflush(stdin);
      memset(cmdin,0,sizeof(cmdin));
    if (read (tdL.cl, &cmdin,sizeof(cmdin)) <= 0)
			{
			  printf("[Thread %d]\n",tdL.idThread);
			  perror ("Eroare la read() de la client.\n");
			
			}
	
	  printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, cmdin);
 
    if(strcmp(cmdin,"quit")==0){
      memset(cmdout,0,sizeof(cmdout));
      strcpy(cmdout,"Bye...");
      if(write(tdL.cl,&cmdout,sizeof(cmdout))<=0){
        printf("[Thread %d] ",tdL.idThread);
		    perror ("[Thread]Eroare la write() catre client.\n");
      }
      break;
    }
      
      if (strstr(cmdin,"login")!=NULL) {
          char * token = strtok(cmdin,"##");
          memset(logincmd,0,sizeof(logincmd));
          memset(username,0,sizeof(username));
          memset(password,0,sizeof(password));
          strcpy(logincmd,token);
          printf("%s\n",logincmd);
          token = strtok(NULL,"##");
          strcpy(username,token);
          printf("%s\n",username);
          token = strtok(NULL,"##");
          strcpy(password,token);
          printf("%s\n",password);
          memset(sql,0,sizeof(sql));
          memset(cmdout,0,sizeof(cmdout));
          sprintf(sql,"SELECT * FROM users WHERE username='%s' AND password='%s';",username,password);
          
          rc = sqlite3_exec(db,sql,callback,str,&zErrMsg);
          
          if (rc!=SQLITE_OK) {
              sprintf(cmdout,"SQL problem: %s\n",zErrMsg);
              sqlite3_free(zErrMsg);
          }
          else if(strstr(str,username)){
              sprintf(cmdout,"Bun venit, %s!\n",username);
              if (strstr(str,"NOT")) {
                  strcat(cmdout,"Rolul tau: user normal!\n");
                  is_user = 1;
              }else{
                  strcat(cmdout,"Rolul tau: admin.\n");
                  is_admin = 1;
              }
          }
          else{
              strcpy(cmdout,"Cont neexistent!\n");
          }
          
      }
      
      if (strstr(cmdin,"register")!=NULL) {
          char * token = strtok(cmdin,"##");
          memset(registercmd,0,sizeof(registercmd));
          memset(username,0,sizeof(username));
          memset(password,0,sizeof(password));
          strcpy(registercmd,token);
          token=strtok(NULL,"##");
          strcpy(username,token);
          token=strtok(NULL,"##");
          strcpy(password,token);
          memset(cmdout,0,sizeof(cmdout));
          memset(sql,0,sizeof(sql));
          sprintf(sql,"SELECT * FROM users WHERE username='%s';",username);
          rc = sqlite3_exec(db,sql,callback,str,&zErrMsg);
          
          if (rc!=SQLITE_OK) {
              sprintf(cmdout,"SQL problem : %s\n",zErrMsg);
              sqlite3_free(zErrMsg);
          }
          
          else if (strstr(str,username)) {
              strcpy(cmdout,"Nume deja existent!Incearca altul!\n");
          }
          else{
                  sprintf(sql,"INSERT INTO users (username,password) VALUES ('%s','%s'); ",username,password);
                  printf("%s\n",sql);
                  rc = sqlite3_exec(db,sql,callback,str,&zErrMsg);
                  if(rc != SQLITE_OK){
                      sprintf(cmdout,"SQL problem: %s\n",zErrMsg);
                      sqlite3_free(zErrMsg);
                  }
                  else
                  {
                      sprintf(cmdout,"Succes la register!");
                  }
              
                     
              }
                     
        }
      
      if (is_admin==1) {
          strcat(cmdout,"\nComenzi disponibilie:\n'add_music title description genre' - adauga piesa\n'vote idpiesa'-voteaza piesa\n'top' - vizualizare top muzical\n'topcat category' - vizualizare top in functie de categorie\nquit\n");
      }
      
      if (is_user==1) {
          strcat(cmdout,"\nComenzi disponibilie:\n'add_music title description genre' - adauga piesa\n'vote idpiesa'-voteaza piesa\n'top' - vizualizare top muzical\n'topcat category' - vizualizare top in functie de categorie\nquit\n");
      }
      
    //trimitem raspunsul clientului...
    if(write(tdL.cl,&cmdout,sizeof(cmdout))<=0){
        printf("[Thread %d] ",tdL.idThread);
		    perror ("[Thread]Eroare la write() catre client.\n");
      } 
    }
                     }
