#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<errno.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
//

#define MAXDS 1024

void*get_addr(struct sockaddr*ref){
	if(ref->sa_family==AF_INET)return&(((struct sockaddr_in*)&ref)->sin_addr);
	return&(((struct sockaddr_in6*)&ref)->sin6_addr);
}
int valid_addr(char*addr_check){
	unsigned char buffer[sizeof(struct in6_addr)];
	return inet_pton(AF_INET,addr_check,buffer)+inet_pton(AF_INET6,addr_check,buffer);
}

int main(int argc,char**argv){
	if(argc<2){
		perror("too few arguments...");
		exit(-1);
	}
	if(argc>4){
		perror("too many arguments...\n");
		exit(-1);
	}

	char addr[INET6_ADDRSTRLEN];
	int result;
	if(argc<4)strcpy(addr,"0.0.0.0");
	else if(!(result=valid_addr(argv[3])))strcpy(addr,"0.0.0.0");
	else strcpy(argv[3],addr);

	struct addrinfo*client,hints;

	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;

	if(getaddrinfo(addr,argv[1],&hints,&client)==-1){
		perror("getaddrinfo()");
		exit(-1);
	}
	int sockfd;
	struct addrinfo*i;
	for(i=client;i;i=i->ai_next){
		if((sockfd=socket(i->ai_family,i->ai_socktype,i->ai_protocol))==-1){
			perror("client: socket");
			continue;
		}
		if(connect(sockfd,i->ai_addr,i->ai_addrlen)==-1){
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}
	if(i==NULL){
		printf("client: failed to connect\n");
		return 2;
	}

	char str[INET6_ADDRSTRLEN],buffer[MAXDS];
	inet_ntop(i->ai_family,get_addr((struct sockaddr*)i->ai_addr),str,sizeof(str));

	freeaddrinfo(client);

	int amount_bytes;	
	if((amount_bytes=recv(sockfd,buffer,MAXDS-1,0))==-1){
		perror("recv()");
		exit(-1);
	}
	buffer[amount_bytes-2]=0;
	printf("client received '%s' from %s\n",buffer,str);
	close(sockfd);

	return 0;
}
