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

	//criar histórico
	
	int res1;
	res1 = open("../tmp/clienteServer", O_WRONLY);


	if(argc == 4 && ((strcmp(argv[1],"execute") && strcmp(argv[2],"-u"))==0)){


	//argumentos do comando
		char* stringArg[52];
	//stringArg[0] : ls
		stringArg[0] = strtok(argv[3]," ");

	//abrir o pipe clienteServer. Guarda os pipes no tmp file


	//faltar modificar para ver se nao ha erro nenhum com falta de argumentos


		//colocar a string comando em stringArg "ls -l ..."
		int i = 0;
		while(stringArg[i++]){
			//printf("%d\n",i);
			//NULL pega na string anterior
			stringArg[i] = strtok(NULL," ");
			//printf("%s\n",stringArg[i]);
		}

		// se o comando for um execute
		
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

		

	} else if(argc ==2 && (strcmp(argv[1],"status")==0) ){
		//comando status
		
		//if (strcmp(argv[1],"status")==0){
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
		

	}else if(strcmp(argv[1],"stats-time")==0){
		
		int argcs1 = argc-2;
		char argcs2[100];
		sprintf(argcs2, "%d",argcs1);
		char stringArg[512];
		strcpy(stringArg,"stats-time ");
		strcat(stringArg,argcs2);
		strcat(stringArg," ");
		

		for (int i = 2; i < argc; i++){

			
			strcat(stringArg,argv[i]);
			if(i==argc-1){

				strcat(stringArg, "\0");

			}else{

				strcat(stringArg," ");
			}
		}
		//stringArg = stats-time pid1 pid2 pid3 etc
		if(argc <= 2){
			char msg3[50];
			strcpy(msg3,"Argumentos errados\n");
			write(1, msg3,strlen(msg3));
			return -1;
		}	else {
			write(res1,stringArg,strlen(stringArg));
		}
		//receber

		char msg2[1024];
		int res6 = open("../tmp/serverCliente",O_RDONLY, 0666);
		int n= read(res6, msg2, sizeof(msg2));
		msg2[n]='\0';
		//printf("%s\n",msg2);
		//printf("%s\n", msg2);
		write(1,msg2,strlen(msg2));
		
		
	}else if(strcmp(argv[1],"stats-command")==0){

		// recebo ./tracer stats-command ls pid pid pid...
		//argc1 número de pids
		int argc1 = argc -3;
		//verificar se o argv[2] é comando
		char command[100];
		snprintf(command, sizeof(command), "which %s > /dev/null", argv[2]);
		//printf("command: %s\n",command);
		if (system(command) == 0) {
			//é um comando

			//argc1 número de pids
			char argcs2[100];
			sprintf(argcs2, "%d",argc1);
			char stringArg[512];
			strcpy(stringArg,"stats-command ");
			strcat(stringArg, argv[2]);
			strcat(stringArg," ");
			strcat(stringArg,argcs2);
			strcat(stringArg," ");

			//printa: stats-command nomecomando nºpids
			//printf("%s\n", stringArg);

			for (int i = 3; i < argc; i++){


				strcat(stringArg,argv[i]);
				if(i==argc-1){

					strcat(stringArg, "\0");

				}else{

					strcat(stringArg," ");
				}
			}
			//caso ./tracer stats-command ls 10 13
			//fico com a string "stats-command ls 2 10 13"
			//posso mandar para o servidor e ele organiza lá
			//printf("%s\n",stringArg);

			if(argc <= 3){
				char msg3[50];
				strcpy(msg3,"Não escreveu pids\n");
				write(1, msg3,strlen(msg3));
				return -1;

			}else{

				write(res1,stringArg,strlen(stringArg));
			} 


			char msg50[1024];
			//printf("tou aqui: %s\n",msg50);
			int res7 = open("../tmp/serverCliente",O_RDONLY, 0666);
			int n= read(res7, msg50, sizeof(msg50));
			msg50[n]='\0';

			//printf("msg:%s \n",msg50);
			write(1,msg50,strlen(msg50));
			write(1,"\n",1);

			
		} else {
			write(1,"not a valid command\n", 20);
			return 1;
		}	

		

		



	}else {

		write(1,"Erro nos argumentos\n",20);
	}

	return 0;
}