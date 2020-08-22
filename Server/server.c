#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


void child(int);

int main(int argc, char *argv[]){  
  int sd, client, portNumber, status;
  struct sockaddr_in servAdd;      // client socket address
  
 if(argc != 2){
    printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
  }
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "Cannot create socket\n");
    exit(1);
  }
  sd = socket(AF_INET, SOCK_STREAM, 0);
  servAdd.sin_family = AF_INET;
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  sscanf(argv[1], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);
  
  bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
  listen(sd, 5);

  while(1){
    client = accept(sd, NULL, NULL);
    printf("Got a client, start chatting\n\n");
    
    if(!fork())
      child(client);

    close(client);
    waitpid(0, &status, WNOHANG);
  }
}

void child(int sd){
	char message[255];
    char filename[255];
        int n, pid;
        int fd,n1;

        char r[] = "start chatting: You have 3 options\n1) get filename\n2) put filename\n3) exit\n"; 
        write(sd, r, sizeof(r));
    int client;
	pid=fork();
	if(pid){              
       while(1){
            if(n=read(sd, message, 255)){
	        message[n]='\0';
            printf("Client/%d: %s", getpid(), message);
	        
	        char * token = strtok(message, " ");
            char reply[255],flag[100];
            int count=0;
            char ack[50];
            if(!strcasecmp(message, "exit")){
    	        kill(pid, SIGTERM);
                kill(getppid(), SIGTERM);
     	        exit(0);
	        }
            else if(!strcasecmp(token, "get")){
                    while (token != NULL)
                    {
                        if(count++==1)
                            strcpy(filename,token);
                         token = strtok (NULL, " ");
                    }
                    filename[strlen(filename)-1] = '\0';
                                      
                   fd = open(filename, O_RDONLY);
                   read(fd, reply, 100);
 
                      if(fd == -1){
                         strcpy(flag,"File doesn't exist\n"); 
                        write(1, flag, strlen(flag));
                        write(sd,"h",1);                        
                    }
                    else{
                        strcpy(flag,"File sent successfully\n"); 
                        write(1, flag, strlen(flag));
                        write(sd, reply, strlen(reply));
                     }
                    
            }
            else if(!strcasecmp(token, "put")){    
                    while (token != NULL)
                    {
                        if(count++==1){
                            strcpy(filename,token);
                            break;
                          }
                         token = strtok (NULL, " ");
                    }
                    filename[strlen(filename)-1] = '\0';
                    
                    
                    n1=read(sd, reply, 255);
                    
                     if(n1 <= 1){
                         printf("File doesn't exist at client side\n");
                     }
                     else{
                          fd = open(filename,O_CREAT|O_WRONLY,0777); 
                          write(fd,reply,strlen(reply));
                          printf("File downloaded\n");
                        }                     
            }
            else{
               strcpy(message,"invalid request from client");
               write(sd, message, strlen(message)+1);
            } 
            printf("\n");
	   }
      } 
    }
 }

