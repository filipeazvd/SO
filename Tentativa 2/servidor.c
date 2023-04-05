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

	char comando[16];
	int pid;
	int seg;
	int milseg;

}infPrograma;

int main(int argc, char* argv[]){

	char buf[512];
	int res;
	int res1;
	
	

	//so fecha para ler qnd todos os clientes fecham para escrever
	while((res1 = open("clienteServer",O_RDONLY))){
		


		if (res1 == -1){
			
			write(1, "Erro abrir ficheiro\n",21);
			return -1;
		}

		int n;
		n = read(res1,buf,sizeof(buf));
		buf[n]= '\0';


		

		if(strncmp(buf,"execute",7)==0){


			infPrograma.comando = malloc(sizeof(struct infPrograma))

			
			/*
			//leio o pid
			n = read(res1, buf,sizeof(buf));
			buf[n]='\0';
			int pid = atoi(buf);

			//leio os segundos
			n = read(res1,buf,sizeof(buf));
			buf[n]= '\0';
			int seg = atoi(buf);

			//leio os microsgeundos mas passo ja para milisegundos
			n=read(res1,buf,sizeof(buf));
			buf[n]= '\0';
			int milseg = atoi(buf);
			*/
			


			gettimeofday(&gettime2, NULL);
			//comparar tempo : milisegundos
			int res2;
			res2 = (gettime2.tv_sec - seg) * 1000 + ((gettime2.tv_usec/1000) - milseg);

			printf("%d\n", res2);

		}else if (strncmp(comando,"status",6)==0){

			//status
		}

	}

	return 0;

}