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

struct timeval gettime2;



typedef struct infPrograma{

	char *comando;
	int pid;
	int seg;
	int milseg;
	struct infPrograma* prox;

}*infPrograma;

void printlist(infPrograma queue, char inf[]){

	infPrograma aux = queue;

	gettimeofday(&gettime2,NULL);

	for (; aux != NULL;aux=aux->prox){
		
		int res3;
		res3 = (gettime2.tv_sec - aux->seg) * 1000 + ((gettime2.tv_usec - aux->milseg)/1000);

		//printf("%s %d %d %d\n", aux->comando, aux->pid,aux->seg,aux->milseg);
		sprintf(inf + strlen(inf), "%d %s %d ms\n",aux->pid,aux->comando,res3);
	}
	
}

int main(int argc, char* argv[]){



	char buf[512];
	int res1;
	infPrograma queue = NULL;
	mkfifo("clienteServer",0666);

	//so fecha para ler qnd todos os clientes fecham para escrever
	while((res1 = open("clienteServer",O_RDWR))){
		
		if (res1 == -1){
			
			write(1, "Erro abrir ficheiro\n",21);
			return -1;
		}

		int n;
		n = read(res1,buf,sizeof(buf));
		buf[n]= '\0';

		if(strncmp(buf,"execute",7)==0){

			strtok(buf," ");

			//colocar inf na struct
			infPrograma s1 = malloc(sizeof(struct infPrograma));
			s1->comando = strdup(strtok(NULL," "));
			//printf("%s\n",s1->comando);
			//s1->comando = strtok(NULL," ");
			s1->pid = atoi(strtok(NULL, " "));
			s1->seg = atoi(strtok(NULL, " "));
			s1->milseg = atoi(strtok(NULL, " "));

			//colocar struct na list ligada


			s1->prox = queue;
			queue = s1;
			
			


		}else if (strncmp(buf,"status",6)==0){

			//status
			int res2;
			strtok(buf," ");

			char status[512];
			
			
			// recebo ./status serverClientepid

			//printf("%s\n",queue->comando);
			char *serverCliente = strtok(NULL," ");
			//printf("%s\n",queue->comando);
			
			printlist(queue, status);
			//printf("%s\n",serverCliente);
			res2 = open(serverCliente, O_WRONLY);
			write(res2,status,strlen(status));
			//close(res2);

			
			//comparar tempo : milisegundos
			




			//printlist(queue);



		}

	}

	return 0;

}