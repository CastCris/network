#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
//
#include<errno.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
//

#define BACKLOG 10

#define HOST_LEN 30
#define SERV_LEN 30
#define HOST_LEN 30

#define ANSI_COLOR_RED 		"\x1b[31m"
#define ANSI_COLOR_GREEN 	"\x1b[32m"
#define ANSI_COLOR_YELLOW 	"\x1b[33m"
#define ANSI_COLOR_BLUE 	"\x1b[34m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_RESET 	"\x1b[0m"

void ERROR(){
	printf(ANSI_COLOR_RED "ERROR(): %s\n\n"ANSI_COLOR_RESET,strerror(errno));
	exit(EXIT_FAILURE);
}

void display_string(char*inp,int output_size){
	for(size_t i=0;i<output_size;++i)printf("%c",inp[i]);
}

int main(int argc,char**argv){
	printf("\n");
	if(argc<2){
		printf(ANSI_COLOR_RED"Please, chose a port for mount the server...\n\n"ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	if(argc<3){
		printf(ANSI_COLOR_RED"Please, choose the amount message you want receive\n\n"ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	if(argc>3){
		printf(ANSI_COLOR_RED"No there's nothing beyond of the port and the number of messages\n\n"ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}

	printf(ANSI_COLOR_BLUE"Mounting the server\n");

	struct addrinfo*relay,hints;

	memset(&hints,0,sizeof(hints));
	hints.ai_flags=AI_PASSIVE;
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;

	printf(ANSI_COLOR_YELLOW"getaddrinfo(): ");
	if(getaddrinfo(NULL,argv[1],&hints,&relay))ERROR();
	printf(ANSI_COLOR_GREEN"Ok\n");

	printf(ANSI_COLOR_YELLOW"socket(): ");
	int sock;
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1)ERROR();
	printf(ANSI_COLOR_GREEN"OK\n");

	printf(ANSI_COLOR_YELLOW"bind(): ");
	if(bind(sock,relay->ai_addr,relay->ai_addrlen)==-1)ERROR();
	printf(ANSI_COLOR_GREEN"OK\n");

	printf(ANSI_COLOR_YELLOW"listen(): ");
	if(listen(sock,BACKLOG)==-1)ERROR();
	printf(ANSI_COLOR_GREEN"OK\n\n"ANSI_COLOR_RESET);

	printf("Server wake up with success in port %s\n",argv[1]);

	char localhost[HOST_LEN];
	if(gethostname(localhost,sizeof(localhost))==-1)ERROR();
	printf("The your name's host is %s\n",localhost);

	printf("Waiting for connexions...\n\n");
	struct sockaddr_storage their_connection;
	int new_sock;
	socklen_t len_store=sizeof their_connection;
	if((new_sock=accept(sock,(struct sockaddr*)&their_connection,(socklen_t*)&len_store))==-1)ERROR();

	printf(ANSI_COLOR_GREEN"Connection found!\n");
	close(sock);

	struct sockaddr_in who;
	socklen_t size_who=sizeof(who);
	if(getpeername(new_sock,(struct sockaddr*)&who,&size_who)==-1){
		close(new_sock);
		close(sock);
		ERROR();
	}

	char ip[INET_ADDRSTRLEN],hostname[HOST_LEN],service[SERV_LEN];
	inet_ntop(AF_INET,&who.sin_addr,ip,INET_ADDRSTRLEN);

	if(getnameinfo((struct sockaddr*)&who,sizeof(who),hostname,HOST_LEN,service,SERV_LEN,0)==-1)ERROR();

	printf(ANSI_COLOR_BLUE"INFORMATIONS ABOUT SENDER\n<-------------------------->\n"ANSI_COLOR_RESET);
	printf("ADDRESS:%s:%s::%s\nNAMES_HOST:IP::PORT",hostname,ip,service);
	printf(ANSI_COLOR_BLUE"\n*--------------------------*\n"ANSI_COLOR_RESET);

	for(int i=0;i<atoi(argv[2]);++i){
		int len_msg;
		char buffer[1024];

		if((len_msg=recv(new_sock,buffer,1023,0))<1){
			close(new_sock);
			close(sock);
			ERROR();
		}

		printf(ANSI_COLOR_YELLOW "     MESSAGE CONTENT\n"ANSI_COLOR_RESET);
		display_string(buffer,len_msg);
		printf(ANSI_COLOR_BLUE"\nx--------------------------x\n"ANSI_COLOR_RESET);
	}
	printf("\n");
	close(new_sock);
	return 0;
}
