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

infPrograma takeList(infPrograma queue, int pid){

	printf("%d 1\n",(queue==NULL));

	//verificar lista se ta vazia
	if( queue == NULL) return NULL;
	infPrograma pr1 = queue;
	infPrograma pr2;
	//primeiro elem da lista
	//o primeiro elemento se for o msm pid é libertada e o primeiro elem passa a ser o prox
	if( pr1->pid == pid){

		queue = queue->prox;
		free(pr1);
		printf("%d 3\n",(queue==NULL));
		return queue;
	}else { 
	// iterar o resto
		printf("%d 2\n",(queue==NULL));
		for (pr2=pr1->prox; pr2 != NULL; pr2=pr2->prox){

			if (pr2->pid == pid ){
				pr1->prox = pr2->prox;
				free(pr2);
				return queue;
				
			}else {

				pr1=pr1->prox;

			}

		}

	}	return queue;


}

void printlist(infPrograma queue, char inf[]){

	infPrograma aux = queue;

	gettimeofday(&gettime2,NULL);
	printf("%d\n",(aux==NULL));
	for (; aux != NULL;aux=aux->prox){
		
		int res3;
		res3 = (gettime2.tv_sec - aux->seg) * 1000 + ((gettime2.tv_usec - aux->milseg)/1000);
		
		

		sprintf(inf + strlen(inf), "%d %s %d ms\n",aux->pid,aux->comando,res3);


		//printf("%s %d %d %d\n", aux->comando, aux->pid,aux->seg,aux->milseg);
	}
	
}

int main(int argc, char* argv[]){



	char buf[512];
	int res1;
	infPrograma queue = NULL;
	mkfifo("../tmp/clienteServer",0666);

	//so fecha para ler qnd todos os clientes fecham para escrever
	while((res1 = open("../tmp/clienteServer",O_RDWR))){
		
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
			//reset a string
			status[0] = '\0';
			
			
			// recebo ./status serverClientepid

			//printf("%s\n",queue->comando);
			char *serverCliente = strtok(NULL," ");
			//printf("%s\n",queue->comando);
			
			printlist(queue, status);
			/*
			//adicionado novo
			//receber o pid de programa terminado e tirar da lista
			int pidd;
			char string[50];
			l = read(res1, pidd, sizeof(int));
			sprintf(string,"%d",pidd);

			if (strtok(status," ")== string){

			}
			*/


			printf("%s\n",status);

			//printf("%s\n",serverCliente);
			res2 = open(serverCliente, O_WRONLY);
			write(res2,status,strlen(status));
			close(res2);

			//comparar tempo : milisegundos
			
			//printlist(queue);



		} else if (strncmp(buf,"Pid", 3)==0){

			// sabe que terminou o prog com o certo pid
			char* word;
			word = strtok(buf," ");
			word = strtok(NULL, " ");
			int p;
			p = atoi(word);
			printf("nandogay\n");
			//queue nao é um apontador
			queue = takeList(queue, p);


			printf("%d\n",p);
			//strtok(buf," ");
			//buf[n]
		}

	}

	return 0;

}