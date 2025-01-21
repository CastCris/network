#include<stdio.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
//

#define ANSI_COLOR_RED 		"\x1b[31m"
#define ANSI_COLOR_GREEN 	"\x1b[33m"
#define ANSI_COLOR_BLUE 	"\x1b[34m"
#define ANSI_COLOR_MAGENTA	"\x1b[36m"
#define ANSI_COLOR_RESET 	"\x1b[0m"
//
int main(int argc,char**argv){
	printf("\n");
	struct addrinfo*relay,*p,hints;
	//
	memset(&hints,0,sizeof(hints));
	//hints.ai_family=AF_UNSPEC;
	//hints.ai_socktype=SOCK_STREAM;
	//
	for(size_t i=0;i<argc-1;++i){
		int status,these=-1,these_p=-1;
		if((status=getaddrinfo(argv[i+1],NULL,&hints,&relay))){
			fprintf(stderr,ANSI_COLOR_RED "getaddrinfo: %s\n",gai_strerror(status));
			printf(ANSI_COLOR_RESET);
			exit(status);
		}
		printf("The linked list of connexion(?) of server %s",argv[i+1]);
		for(struct addrinfo*i=relay;i;i=i->ai_next){
			char ipv_uns[i->ai_addrlen];

			void*addr;
			if(i->ai_family==AF_INET){
				struct sockaddr_in*temp=(struct sockaddr_in*)(i->ai_addr);
				addr=&(temp->sin_addr);
			}else{
				struct sockaddr_in6*temp=(struct sockaddr_in6*)(i->ai_addr);
			   	addr=&(temp->sin6_addr);
			}
			inet_ntop(i->ai_family,addr,ipv_uns,i->ai_addrlen);

			these=i->ai_family==AF_INET;
			if(these!=these_p){
				printf(ANSI_COLOR_GREEN "\nThe connextion(s) now is: ");
				printf(ANSI_COLOR_MAGENTA);
				if(these)printf("IPv4\n");
				else printf("IPv6\n");
				printf(ANSI_COLOR_RESET);
			}
			printf(ANSI_COLOR_BLUE "%s \n"ANSI_COLOR_RESET,ipv_uns);
			these_p=these;
		}
		printf("\n=====================================\n\n");
		freeaddrinfo(relay);
	}
	return 0;
}
