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

void printlist(infPrograma queue){

	infPrograma aux = queue;

	for (; aux != NULL;aux=aux->prox){
		
		printf("%s %d %d %d\n", aux->comando, aux->pid,aux->seg,aux->milseg);
	}
}


int main(int argc, char* argv[]){

	char buf[512];
	int res;
	int res1;
	infPrograma queue = NULL;
	

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
			s1->comando = strtok(NULL," ");
			//s1->comando = strtok(NULL," ");
			s1->pid = atoi(strtok(NULL, " "));
			s1->seg = atoi(strtok(NULL, " "));
			s1->milseg = atoi(strtok(NULL, " "));

			//colocar struct na list ligada

			s1->prox = queue;
			queue = s1;

			
			/*
			gettimeofday(&gettime2, NULL);
			//comparar tempo : milisegundos
			int res2;
			res2 = (gettime2.tv_sec - seg) * 1000 + ((gettime2.tv_usec/1000) - milseg);

			printf("%d\n", res2);
			*/


		}else if (strncmp(buf,"status",6)==0){

			//status
			
			strtok(buf," ");
			char serverCliente[50] = strtok(NULL," ");
			res2 = open(serverCliente, "O_WRONLY");
			

			

			printlist(queue);



		}

	}

	return 0;

}