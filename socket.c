// I/O
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//
/*
 *	Functions:
 * 		getaddrinfo,socket,bind,connect  
 *	Structs:
 * 		 addrinfo,sockaddr_in(6),bind
 *
*/
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
// Addictional
#include<errno.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
//

#define MYPORT "3490"

#define ANSI_COLOR_RED 		"\x1b[31m"
#define ANSI_COLOR_GREEN 	"\x1b[32m"
#define ANSI_COLOR_YELLOW 	"\x1b[33m"
#define ANSI_COLOR_BLUE 	"\x1b[34m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_RESET 	"\x1b[0m"

void ERROR(int out){
	printf(ANSI_COLOR_RED "ERROR(): %s\n\n" ANSI_COLOR_RESET,strerror(errno));
	exit(out);
}

int main(int argc,char**argv){
	printf("\n");

	struct addrinfo*relay,hints;

	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	
	for(size_t i=0;i<argc-1;++i){
		printf(ANSI_COLOR_YELLOW "Try connexion with %s\n" ANSI_COLOR_RESET,argv[i+1]);

		printf("getaddrinfo(): ");
		if(getaddrinfo(argv[i+1],MYPORT,&hints,&relay))ERROR(-1);
		printf(ANSI_COLOR_GREEN "Ok\nEstabilish the connexion...\n" ANSI_COLOR_RESET);

		printf("socket(): ");
		int sock=socket(relay->ai_family,relay->ai_socktype,0);
		if(sock==-1)ERROR(-1);
		printf(ANSI_COLOR_GREEN "Ok\n" ANSI_COLOR_RESET);
		
		printf("connect(): ");
		relay->ai_addr->sa_family=0;//For some reason the value defualt is brojen ¯\_(ツ)_/¯¯
	   if(connect(sock,relay->ai_addr,relay->ai_addrlen)==-1)ERROR(-1);	 
	   printf(ANSI_COLOR_GREEN "Ok\n");
	   printf(ANSI_COLOR_MAGENTA "Yeap, you are connect now!\n\n" ANSI_COLOR_RESET);
	}

	return 0;
}
