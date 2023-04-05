#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/stat.h>

struct timeval gettime1;

int main(int argc, char* argv[]){

	char buf[512];
	char* stringArg[52];
	stringArg[0] = strtok(argv[3]," ");
	

	mkfifo("clienteServer",0666);

	int res1;
	res1 = open("clienteServer", O_WRONLY);
	//res2 = open("serverCliente", O_RDONLY);

	if(argc >= 4){

		int i = 0;

		while(stringArg[i++]){
				//printf("%d\n",i);
				//NULL pega na string anterior
				stringArg[i] = strtok(NULL," ");
				//printf("%s\n",stringArg[i]);
			}

		if(strcmp(argv[1],"execute")==0){

			pid_t pid = fork();

			if(pid == 0){

				char msg[512];
				char comando[30];
				// ./cliente execute -u "ls -l"
				//stringArg[0] = ls
				
				strcpy(comando,stringArg[0]);
				//strcpy(comando,"\n");
				//write(1,comando,strlen(comando));
				

				//mandar o comando para o pipe
				//write(res1, stringArg[0], strlen(stringArg[0]));

				char pidF[8];
				sprintf(pidF,"%d",getpid());
				//mandar pid para o pipe
				//write(res1,pidF,strlen(pidF));

				gettimeofday(&gettime1,NULL);
				char sec[20];
				char microsec[20];

				sprintf(sec,"%ld",gettime1.tv_sec);
				sprintf(microsec,"%ld",gettime1.tv_usec);

				strcpy(msg,argv[1]);
				strcat(msg," ");
				strcat(msg,comando);
				strcat(msg," ");
				strcat(msg,pidF);
				strcat(msg," ");
				strcat(msg,sec);
				strcat(msg," ");
				strcat(msg,microsec);

				write(res1, msg, strlen(msg));


				//mandar para o pipe segundos e microsegundos
				//write(res1,sec,strlen(sec));
				//write(res1,microsec,strlen(microsec));
				printf("%s, %s, %s, %s\n",stringArg[0],pidF,sec,microsec);
				execvp(stringArg[0],stringArg);

			}

		}
	
	} else if(argc <=2){

		if (strcmp(argv[1],"status")==0){

			char c[40];
			sprintf(c,"serverCliente%d",getpid());
			mkfifo(c,0666);

			char msg2[100];
			sprintf(msg2,"status %s",c);
			write(res1,msg2,strlen(msg2));



		}	else{

			return 1;
		}
	}

	wait(NULL);


	return 0;
}