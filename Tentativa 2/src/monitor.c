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
#include <string.h>


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
		strcpy(pathInteiro,"../");
		strcat(pathInteiro,path);
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

	//colocar o programa para ./monitor PIDS-folder com argv[1] = etc..
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

			// ls pid seg mil
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
				//buf: Pid 12323
				char* word;
				word = strtok(buf," ");
				word = strtok(NULL, " ");
				char* wordaux = word;
				int p;
				p = atoi(word);
				


			//queue nao é um apontador
				queue = takeList(queue, p, path);

				
				
				
				char pathficheiro[100] = "/home/filipe/Desktop/SO/Trab Pratico/SO/Tentativa 2/PIDS-folder/";

				strcat(pathficheiro,wordaux);
				strcat(pathficheiro,".txt");

				int hist = open(pathficheiro, O_RDONLY, 0666);

				if(hist > 0){
						int ler;
						//char buf4[1024];
						char buf5[1024];
						buf5[0]= '\0';
						ler = read(hist,buf5,sizeof(buf5));
						buf5[ler]='\0';
						//pid comando tempo ms
						strtok(buf5," ");
						strtok(NULL, " ");
						char *tempo = strtok(NULL, " ");
						
						char pipe[50];
						strcpy(pipe,"../tmp/");
						strcat(pipe,wordaux);
						
						int res6 = open(pipe, O_WRONLY, 0666); 
						
						write(res6, tempo,strlen(tempo));		

				}

			} else if(strncmp(buf,"stats-time",10)==0){
				
				//no buffer: stats-time ../tmp/serverClientepid nºpids pid1 pid2 pid3 etc
				// pipe para enviar informacoes serverCliente do comando status-time

				
				strtok(buf," ");
				char *nomePipe = strtok(NULL," ");

				char *s = strtok(NULL," ");
				
				//tenho nº de pids no argc
				int argc = atoi(s);
				
				char path2[argc][100];
				char pids[argc][100];

				for (int i = 0; i < argc ; i++) {
					memset(path2[i], 0, sizeof(path2[i]));
					memset(pids[i], 0, sizeof(pids[i]));

					
					char *d = strtok(NULL," ");
					strcat(path2[i],d);

					strcat(pids[i],path2[i]);
					
				}

    			// concatenate path and pid

				
				int soma=0;
				for (int i = 0; i < argc; i++){
					//./monitor ../PIDS-folder
					char pathficheiro[100] = "/home/filipe/Desktop/SO/Trab Pratico/SO/Tentativa 2/PIDS-folder/";
					
					strcat(pathficheiro,pids[i]);
					
					strcat(pathficheiro,".txt");
					
					
					
					
					int hist = open(pathficheiro, O_RDONLY, 0666);
					
					if(hist > 0){
						int ler;
						char buf4[1024];
						//buf4[0]='\0';
						//vou ler os pids para o buf5
						char buf5[1024];
						buf5[0]= '\0';
						ler = read(hist,buf4,sizeof(buf4));
						strcpy(buf5,buf4);
						
						
						buf5[ler]='\0';
						//pid comando tempo ms
						strtok(buf5," ");
						strtok(NULL, " ");
						char *tempo = strtok(NULL, " ");
			
						int m = atoi(tempo);
						soma += m;
						

					}

					
				}
				if(soma > 0){

					char soma1[50];
					char somaString[50];
					somaString[0]='\0';
					strcat(somaString,"Total execution time is ");
					sprintf(soma1, "%d ",soma);
					strcat(somaString, soma1);
					
					strcat(somaString,"ms");
					strcat(somaString,"\n");




					int res5 = open(nomePipe, O_WRONLY, 0666); 
					
					write(res5, somaString, strlen(somaString));
					strcat(somaString,"\0");



				}else {

					int res5 = open(nomePipe, O_WRONLY, 0666); 
					char msg2[50];
					strcpy(msg2,"Nenhum dos pids terminou ou existe");
					strcat(msg2,"\n");
					write(res5,msg2,strlen(msg2));
					close(res5);


				}


			}else if(strncmp(buf,"stats-command",13)==0){

				//string no buffer: stats-command pidprocesso ls 2 1230 1323
				strtok(buf," ");
				//comando
				char *ppidd = strtok(NULL," ");
				char* com =strtok(NULL," ");
			
				char* d =strtok(NULL," ");
				//qnts pids tem
				int argc = atoi(d);

				char pipe[50];
				strcpy(pipe,"../tmp/");
				strcat(pipe,ppidd);
				
				char pids[argc][100];
				//meter os pids recebidos em pid[i]
				for (int i = 0; i < argc ; i++) {
					//memset(path2[i], 0, sizeof(path2[i]));
					memset(pids[i], 0, sizeof(pids[i]));

					
					char *d = strtok(NULL," ");
					strcat(pids[i],d);
					
				}

				int count = 0;
				char mensagem[512];
				mensagem[0]= '\0';

				for (int i = 0; i < argc; i++){
					//./monitor ../PIDS-folder
					char pathficheiro[100] = "/home/filipe/Desktop/SO/Trab Pratico/SO/Tentativa 2/PIDS-folder/";
					
					strcat(pathficheiro,pids[i]);
					strcat(pathficheiro,".txt");

					int hist = open(pathficheiro, O_RDONLY, 0666);
					char msg[50];
					msg[0]='\0';

					if(hist > 0){
						int ler;
						char buf4[1024];
						char buf5[1024];
						buf5[0]= '\0';
						ler = read(hist,buf4,sizeof(buf4));
						strcpy(buf5,buf4);
						buf5[ler]='\0';
						//pid comando tempo ms

						strtok(buf5," ");
						//comando no tempo
						char *comando = strtok(NULL, " ");
						//tenho o comando na msg
						strcat(msg,comando);
						
						if(strcmp(msg,com)==0){
							count++;
						}
						


					}else{
						
						int res5 = open(pipe, O_WRONLY, 0666); 
						char msg2[50];
						strcpy(msg2,"Programa ");
						strcat(msg2,pids[i]);
						strcat(msg2," não existe");
						strcat(msg2,"\n");
						write(res5,msg2,strlen(msg2));
						close(res5);
					}



				}
				
				char contador[10];
				sprintf(contador, "%d", count);
				strcat(mensagem, "O comando foi executado ");
				strcat(mensagem, contador);
				strcat(mensagem, " vezes");
				

				int res10 = open(pipe, O_WRONLY, 0666); 
				write(res10, mensagem, strlen(mensagem));
				strcat(mensagem,"\0");


			}else if(strncmp(buf,"stats-uniq",10)==0){

				//recebo stats-uniq pidgeral npids pid1 pid2 pid3
				
				strtok(buf," ");
				//s apontar para o nºpids
				char *pidgeral = strtok(NULL," ");
				char *s = strtok(NULL," ");
				int n = atoi(s);

				char pids[n][100];
				//meter os pids recebidos em pid[i]
				for (int i = 0; i < n ; i++) {
					//memset(path2[i], 0, sizeof(path2[i]));
					memset(pids[i], 0, sizeof(pids[i]));

					
					char *d = strtok(NULL," ");
					strcat(pids[i],d);
					
				}

				char mensagemFinal[512];
				mensagemFinal[0]='\0';
				for (int i = 0; i < n; i++){
					//./monitor ../PIDS-folder
					char pathficheiro[100] = "/home/filipe/Desktop/SO/Trab Pratico/SO/Tentativa 2/PIDS-folder/";
					
					strcat(pathficheiro,pids[i]);
					strcat(pathficheiro,".txt");

					int hist = open(pathficheiro, O_RDONLY, 0666);
					

					if(hist > 0){

						//char buf4[1024];
						char buf5[1024];
						//buf5[0]= '\0';
						int ler = read(hist,buf5,sizeof(buf5));
						//strcpy(buf5,buf4);
						buf5[ler]='\0';
						//pid comando tempo ms
						
						strtok(buf5," ");
						//comando no tempo
						char *comando = strtok(NULL, " ");
						//tenho o comando na msg
						
						
						
						if(strstr(mensagemFinal,comando)==NULL){
							strcat(mensagemFinal,comando);
							strcat(mensagemFinal,"\n");	
							
						}
						

					}
				}
				
				char pipe[50];
				strcpy(pipe,"../tmp/");
				strcat(pipe,pidgeral);


				if(strlen(mensagemFinal)==0){

					
					int res11 = open(pipe, O_WRONLY, 0666); 
					write(res11, "Nenhum Pid estava correto\n",26);
				}

				int res10 = open(pipe, O_WRONLY, 0666); 
				write(res10, mensagemFinal, strlen(mensagemFinal));
				strcat(mensagemFinal,"\0");




			}

		}
		//unlink("../tmp/clienteServer");
		
		
		
	}
	return 0;

}