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
struct timeval gettime2;

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
			
			//NULL pega na string anterior
			stringArg[i] = strtok(NULL," ");
			
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
			write(1,"Running PID ",12);
			write(1,pidF,strlen(pidF));
			write(1,"\n",1);
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

			char pipe[50];
			sprintf(pipe,"../tmp/%s",pidd);
			mkfifo(pipe,0666);

			

			write(res1,a,strlen(a));

			//talvez tenha que esperar
			
			char buffer[50];
			
			int res7 = open(pipe, O_RDONLY, 0666);
			int res8 = read(res7,buffer,50);
			buffer[res8]='\0';
			


			write(1,"\n",1);
			write(1,"Ended in ",9);
			write(1,buffer,strlen(buffer));
			write(1," ms\n",4);
			close(res7);
			unlink(pipe);


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
			// \0 end of file no buffer
		buf2[n]='\0';
			//escrever no stdout o status
		write(1, buf2, strlen(buf2));
		unlink(c);
		

	}else if(strcmp(argv[1],"stats-time")==0){
		
		//preciso de criar um pipe para cada comando para serverCliente ser unico
		char c[40];
		sprintf(c,"../tmp/serverCliente%d",getpid());
		mkfifo(c,0666);
		//char mensagem[100];

		//mensagem que quero enviar é: ./tracer stats-time ../tmp/serverClientepid pid1 pid2 etc...
		//
		int argcs1 = argc-2;
		char argcs2[100];
		//no argcs2 tenho o numero de pids que quero ver 
		sprintf(argcs2, "%d",argcs1);
		char stringArg[512];
		strcpy(stringArg,"stats-time ");
		strcat(stringArg,c);
		strcat(stringArg," ");
		strcat(stringArg,argcs2);
		strcat(stringArg," ");
		
		//printf("%s\n",stringArg);
		//tenho no stringArg: stats-time ../tmp/serverClientepid nºpids

		for (int i = 2; i < argc; i++){

			
			strcat(stringArg,argv[i]);
			if(i==argc-1){

				strcat(stringArg, "\0");

			}else{

				strcat(stringArg," ");
			}
		}

		


		//stringArg: stats-time ../tmp/serverClientepid nºpids pid1 pid2 pid3 etc
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
		int res6 = open(c,O_RDONLY, 0666);
		int n= read(res6, msg2, sizeof(msg2));
		msg2[n]='\0';
		
		write(1,msg2,strlen(msg2));
		unlink(c);
		
		
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
			int p = getpid();
			char pp[15];
			sprintf(pp,"%d",p);
			strcpy(stringArg,"stats-command ");
			strcat(stringArg,pp);
			strcat(stringArg," ");
			strcat(stringArg, argv[2]);
			strcat(stringArg," ");
			strcat(stringArg,argcs2);
			strcat(stringArg," ");

			//printa: stats-command pid nomecomando nºpids 12 12 12 12 12
			

			for (int i = 3; i < argc; i++){


				strcat(stringArg,argv[i]);
				if(i==argc-1){

					strcat(stringArg, "\0");

				}else{

					strcat(stringArg," ");
				}
			}


			char pipe[50];
			strcpy(pipe,"../tmp/");
			strcat(pipe,pp);
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

				
				mkfifo(pipe,0666);
				write(res1,stringArg,strlen(stringArg));
			} 

			//receber

			char msg50[1024];
			//printf("tou aqui: %s\n",msg50);
			int res7 = open(pipe,O_RDONLY, 0666);
			int n= read(res7, msg50, sizeof(msg50));
			msg50[n]='\0';

			//printf("msg:%s \n",msg50);
			write(1,msg50,strlen(msg50));
			write(1,"\n",1);
			unlink(pipe);

			
		} else {
			write(1,"not a valid command\n", 20);
			return 1;
		}	

	

	}else if(strcmp(argv[1],"stats-uniq")==0){


		//./tracer stats-uniq pid1 pid2 pid3 etc...
		int cc= getpid();
		char cc2[50];
		//tenho o pid do processo geral em cc2
		sprintf(cc2,"%d",cc);

		int argcs1 = argc-2;
		char argcs2[100];
		sprintf(argcs2, "%d",argcs1);
		char stringArg[512];
		strcpy(stringArg,"stats-uniq ");
		strcat(stringArg,cc2);
		strcat(stringArg," ");
		strcat(stringArg,argcs2);
		strcat(stringArg," ");

		//stringArg: stats-uniq pidprocesso nªpids

		for (int i = 2; i < argc; i++){

			
			strcat(stringArg,argv[i]);
			if(i==argc-1){

				strcat(stringArg, "\0");

			}else{

				strcat(stringArg," ");
			}
		}
		

		char pipe[50];
		strcpy(pipe,"../tmp/");
		strcat(pipe,cc2);

		//stringArg = stats-uniq pidgeral npids pid1 pid2 pid3 etc
		if(argc <= 2){
			char msg3[50];
			strcpy(msg3,"Argumentos errados\n");
			write(1, msg3,strlen(msg3));
			return -1;
		}	else {
			//printf("%s\n",stringArg);
			mkfifo(pipe,0666);
			write(res1,stringArg,strlen(stringArg));
		}


		//receber
		//se tiver 3 programas 1st ls 2nd ls e 3rd cat ao procurar stats-uniq 1st 2nd 3rd aparece so ls mas se meter cat primeiro aparece direito
		char msg2[1024];
		int res6 = open(pipe,O_RDONLY, 0666);
		int n= read(res6, msg2, sizeof(msg2));
		msg2[n]='\0';
		
		write(1,msg2,strlen(msg2));
		unlink(pipe);
	}else {

		write(1,"Erro nos argumentos\n",20);
	}

	return 0;
}