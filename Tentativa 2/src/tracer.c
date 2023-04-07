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

	//argumentos do comando
	char* stringArg[52];
	//stringArg[0] : ls
	stringArg[0] = strtok(argv[3]," ");
	
	//abrir o pipe clienteServer. Guarda os pipes no tmp file
	int res1;
	res1 = open("../tmp/clienteServer", O_WRONLY);
	

	//faltar modificar para ver se nao ha erro nenhum com falta de argumentos

	if(argc >= 4){

		//colocar a string comando em stringArg "ls -l ..."
		int i = 0;
		while(stringArg[i++]){
				//printf("%d\n",i);
				//NULL pega na string anterior
				stringArg[i] = strtok(NULL," ");
				//printf("%s\n",stringArg[i]);
			}

		// se o comando for um execute
		if(strcmp(argv[1],"execute")==0){
			//cria filho 
			pid_t pid = fork();

			if(pid == 0){

				//msg que vou enviar com a inf do execute
				char msg[512];
				char comando[30];
				//comando vai ter o "ls -l ..."
				strcpy(comando,stringArg[0]);
				
				//obter o pid do processo filho e colocar em pidF
				char pidF[8];
				sprintf(pidF,"%d",getpid());
				
				//tempo atual
				gettimeofday(&gettime1,NULL);
				char sec[20];
				char microsec[20];

				//colocar o tempo numa string
				sprintf(sec,"%ld",gettime1.tv_sec);
				sprintf(microsec,"%ld",gettime1.tv_usec);

				//colocar toda a informacao do execute numa string para enviar para o server
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

				//executar o comando no cliente
				execvp(stringArg[0],stringArg);

			}else{

				//pai
				//obtem o pid do filho atraves do status e manda a informacao do programa terminado ao server
				int status;
				int terminated_pid = wait(&status);
				char pidd[20];
				sprintf(pidd,"%d",terminated_pid);
				char a[50];
				strcpy(a,"Pid");
				strcat(a," ");
				strcat(a,pidd);

				write(res1,a,strlen(a));
			}

		}
	
	} else if(argc <=2){
		//comando status
		if (strcmp(argv[1],"status")==0){
			//criar um pipe para pedir o status ao server, este vai saber que pipe é atraves do pid
			char c[40];
			sprintf(c,"../tmp/serverCliente%d",getpid());
			
			mkfifo(c,0666);

			//mandar para o server

			char msg2[100];
			sprintf(msg2,"status %s",c);

			//msg2: status ../tmp/serverCliente3164
			write(res1,msg2,strlen(msg2));

			//recebe de volta a informação do status que diz os programas a executar no momento
			int n,res2;
			char buf2[512];
			//abre o pipe
			res2 = open(c, O_RDONLY);
			n = read(res2,buf2,sizeof(buf2));
			//\0 end of file no buffer
			buf2[n]='\0';
			//escrever no stdout o status
			write(1, buf2, strlen(buf2));


		}	else{

			return 1;
		}
	}
	
	
	

	return 0;
}