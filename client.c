#include<stdio.h>
#include<string.h>
#include<stdlib.h>
//
#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
//
#include<errno.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#define HOST_LEN 10
#define SERV_LEN 10

#define ANSI_COLOR_RED 		"\x1b[31m"
#define ANSI_COLOR_GREEN 	"\x1b[32m"
#define ANSI_COLOR_YELLOW 	"\x1b[33m"
#define ANSI_COLOR_BLUE 	"\x1b[34m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_RESET 	"\x1b[0m"


void ERROR(char*add_msg){
	if(strlen(add_msg))printf(ANSI_COLOR_RED"%s",add_msg);
	printf(ANSI_COLOR_RED"ERROR(): %s\n\n"ANSI_COLOR_RESET,strerror(errno));
	exit(EXIT_FAILURE);
}

int main(int argc,char**argv){
	printf("\n");
	if(argc<2)ERROR("Please, insert the number of port for connect with the localhost\n");
	if(argc<3)ERROR("Please, insert the number of queries\n");
	if(argc>4)ERROR("There's nothing beyond of these arguments...\n");

	char ipv4[INET_ADDRSTRLEN];
	if(argc<4)strcpy(ipv4,"0.0.0.0");
	else strcpy(ipv4,argv[3]);

	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(ipv4);
	addr.sin_port=htons(atoi(argv[1]));

	printf(ANSI_COLOR_YELLOW"socket(): ");
	short int sock=socket(PF_INET,SOCK_STREAM,0);
	if(sock==-1)ERROR(NULL);
	printf(ANSI_COLOR_GREEN"OK\n");

	printf(ANSI_COLOR_YELLOW"connect(): ");
	if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))==-1)ERROR(NULL);
	printf(ANSI_COLOR_GREEN"OK\n"ANSI_COLOR_RESET);

	socklen_t addr_size=sizeof(&addr);
	getpeername(sock,(struct sockaddr*)&addr,&addr_size);
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET,&addr.sin_addr,ip,INET_ADDRSTRLEN);

	char hostname[HOST_LEN],serviname[SERV_LEN];
	if(getnameinfo((struct sockaddr*)&addr,sizeof(addr),hostname,sizeof(hostname),serviname,sizeof(serviname),0)==-1)ERROR(NULL);

	printf("Now you are connect!\n");
	printf(ANSI_COLOR_BLUE"\nINFOMATIONS ABOUT RECIPIENT:\n");
	printf(ANSI_COLOR_RESET "ADDRESS:%s:%s:%s",hostname,ip,serviname);
	printf(ANSI_COLOR_BLUE"\n\n--------------------------\n"ANSI_COLOR_RESET);

	for(int i=0;i<atoi(argv[2]);++i){
		int status;
		char message[1024];
		printf("ENTER WITH THE MESSAGE: ");
		fgets(message,1024,stdin);
		if((status=send(sock,message,strlen(message),0))<1)ERROR(NULL);
		
		if(status!=strlen(message))printf(ANSI_COLOR_YELLOW"The function send can't hadle with the sizes massage...\n"ANSI_COLOR_RESET);
		printf(ANSI_COLOR_GREEN"Message devired, check the outputs server\n\n"ANSI_COLOR_RESET);
	}
	close(sock);
	return 0;
}
