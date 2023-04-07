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





typedef struct infPrograma{

	char *comando;
	int pid;
	int seg;
	int milseg;
	struct infPrograma* prox;

}*infPrograma;


//funcao para tirar um elemento da lista ligada
infPrograma takeList(infPrograma queue, int pid, char* path ){

	//verificar lista se ta vazia
	if( queue == NULL) return NULL;

	//para iterar e retirar é preciso dois apontadores
	infPrograma pr1 = queue;
	infPrograma pr2;
	
	//verificar com o pid que recebe(o pid do programa terminado) verifica se terminou,
	// se sim o proximo passa a ser o primeiro e o terminado é libertado
	if( pr1->pid == pid){

		struct timeval gettime3;
		//path neste
		char pathInteiro[50];
		char idProcesso[10]; 
		sprintf(idProcesso,"%d",pid);
		strcpy(pathInteiro,path);
		strcat(pathInteiro, "/");
		strcat(pathInteiro,idProcesso);
		strcat(pathInteiro,".txt");

		gettimeofday(&gettime3,NULL);

		int res3;
		res3 = (gettime3.tv_sec - pr1->seg) * 1000 + ((gettime3.tv_usec - pr1->milseg)/1000);


		int folder = open(pathInteiro, O_RDWR | O_TRUNC | O_CREAT ,0666);

		char buf[512];
		sprintf(buf, "%d %s %d ms", pr1->pid,pr1->comando, res3);

		write(folder, buf,strlen(buf));
		close(folder);

		

		queue = queue->prox;
		free(pr1);
		
		return queue;

	}else { 

		// caso o primeiro não esteja terminado vai iterar no resto da lista
		// para percorrer uma lista ligada é preciso o pr1 a apontar para o primeiro elemento
		// pr2 a apontar para o segundo elemento
		//vai percorrer a lista ate o pr2 ser NULL (pr2 vai percorrer a lista)
		for (pr2=pr1->prox; pr2 != NULL; pr2=pr2->prox){

			//verifica o pid
			if (pr2->pid == pid ){
				//neste caso tamos a verificar se o segundo elemento ja terminou
				//se terminou temos que pr1->prox(que é o segundo elemento) passa a ser o seguinte
				// e liberta-se o pr2 pq ja terminou
				pr1->prox = pr2->prox;
				free(pr2);
				return queue;
				
			}else {
				//caso nao tenha terminado passa-se para o proximo caso
				//o pr1 avança e o pr2 avança no ciclo for
				pr1=pr1->prox;

			}

		}

	}	return queue;


}

//funcao que vai printar as informacoes da queue e mandar para o cliente
void printlist(infPrograma queue, char inf[]){
	
	struct timeval gettime2;
	//criamos uma aux para nao mudarmos a lista em si
	infPrograma aux = queue;
	//obtemos o tempo atual
	gettimeofday(&gettime2,NULL);
	

	for (; aux != NULL;aux=aux->prox){
		
		//calculo para comparar o tempo atual com o tempo inical de execucao recebido do cliente
		int res3;
		res3 = (gettime2.tv_sec - aux->seg) * 1000 + ((gettime2.tv_usec - aux->milseg)/1000);
		
		
		//colocamos na string para enviar
		sprintf(inf + strlen(inf), "%d %s %d ms\n",aux->pid,aux->comando,res3);


		

	}
	
}

int main(int argc, char* argv[]){


	if (argc == 2){

		char *path = argv[1];

		char buf[512];
		int res1;

	//lista vazia criada para colocar structs com o status
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


			//colocar inf numa struct
				infPrograma s1 = malloc(sizeof(struct infPrograma));
				s1->comando = strdup(strtok(NULL," "));
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
			//buf tá no token status
			//vai ler: status ../tmp/serverCliente39213
				char status[512];
			//reset a string
				status[0] = '\0';

			// vai ler a partir de ../tmp/serverCliente39213
				char *serverCliente = strtok(NULL," ");

				printlist(queue, status);

			//abre o pipe serverCliente e escreve o status. Fecha para nao ficar a espera
				res2 = open(serverCliente, O_WRONLY);
				write(res2,status,strlen(status));
				close(res2);


			} else if (strncmp(buf,"Pid", 3)==0){

			//recebe do cliente pids do programa terminado
			//coloca numa variavel e manda para a funcao que retira da lista
				char* word;
				word = strtok(buf," ");
				word = strtok(NULL, " ");
				int p;
				p = atoi(word);
				printf("gasdgdgas\n");
				

			//queue nao é um apontador
				queue = takeList(queue, p, path);





			}

		}

	}
	return 0;

}