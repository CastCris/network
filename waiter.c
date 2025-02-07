#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<errno.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#define PORT "8080"
#define BACKLOG 10

void	*get_addr(struct addrinfo*ref){
	if(ref->ai_family==AF_INET)return&((struct sockaddr_in*)ref)->sin_addr;
	return&(((struct sockaddr_in6*)ref)->sin6_addr);
}

void sigchld_handler(int s){
	int saved_errno=errno;
	while(waitpid(-1,NULL,WNOHANG)>0);
	errno=saved_errno;
}

int main(){
	struct addrinfo*relay,hints,*check;

	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;

	if(getaddrinfo(NULL,PORT,&hints,&relay)){
		perror("getaddrinfo():");
		exit(-1);
	}

	int sock;
	for(check=relay;check;check=check->ai_next){
		sock=socket(check->ai_family,check->ai_socktype,check->ai_protocol);
		if(sock==-1){
			perror("sock():");
			continue;
		}

		int yes=1;
		if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("setsockopt():");
			exit(-1);
		}

		if(bind(sock,check->ai_addr,check->ai_addrlen)==-1){
			close(sock);
			perror("bind():");
			continue;
		}
		break;
	}
	freeaddrinfo(relay);

	if(!check){
		fprintf(stderr,"server failed tor bind\n");
		exit(1);
	}

	if(listen(sock,BACKLOG)==-1){
		perror("listen():");
		exit(-1);
	}

	struct sigaction sa;
	sa.sa_handler=sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=SA_RESTART;
	if(sigaction(SIGCHLD,&sa,NULL)==-1){
		perror("sigaction");
		exit(-1);
	}

	printf("Server waiting for connection...\n");
	while(1){
		struct sockaddr_storage their_addr;
		socklen_t sin_size=sizeof(their_addr);
		int new_sock=accept(sock,(struct sockaddr*)&their_addr,&sin_size);
		if(new_sock==-1){
			perror("accept():");
			continue;
		}

		char ip[INET6_ADDRSTRLEN];
		inet_ntop(their_addr.ss_family,get_addr((struct addrinfo*)&their_addr),ip,sizeof(ip));

		if(!fork()){
			close(sock);
			char msg[]="Welcome to localhost!\n";
			if(send(new_sock,msg,sizeof(msg),0)==-1)perror("send");
			close(new_sock);
			exit(0);
		}
		close(new_sock);
	}
	return 0;
}
