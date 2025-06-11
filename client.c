#include "client.h"
int main(int argc, char* argv[])
{
	int socket_fd, numRecv;
	char sendbuf[BUF_SIZE], recvbuf[BUF_SIZE];
	struct sockaddr_in servaddr;
	
	if( argc != 2){
		printf("usage: ./client <ipaddress>\n");
		exit(0);	
	} 
	//initialize socket
	if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("create socket error: %s(errno: %d)\n",strerror(errno), errno);
		exit(0);
	}
	//initialize servaddr
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8000);
	if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) == -1){
		printf("inet_pton error for %s\n",argv[1]);
		exit(0);
	}
	//connect server
	if( connect(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		printf("connect error: %s(errno: %d)\n",strerror(errno), errno);
		exit(0);
	}
	//send username to server
	printf("Login(username): \n");
	fgets(sendbuf, BUF_SIZE, stdin);
	if( send(socket_fd, sendbuf, strlen(sendbuf), 0) == -1){
		printf("send msg error: %s(errno: %d)\n",strerror(errno), errno);
		exit(0);
	}
	if( (numRecv = recv(socket_fd, recvbuf, BUF_SIZE, 0)) == -1){
		perror("recv error");
		exit(0);	
	}
	if (strncmp(recvbuf, "faile", numRecv-1) == 0){
		printf("Login Error : Wrong UserName\n");
		exit(0);
	}
	else{
		printf("Login Success\n");
	}
	
	while(1){
		//send requset to server
		printf("Please Input Your Request : ");
		fgets(sendbuf, BUF_SIZE, stdin);
		if( send(socket_fd, sendbuf, strlen(sendbuf), 0) == -1){
			printf("send msg error: %s(errno: %d)\n",strerror(errno), errno);
			exit(0);
		}
		//handle respond
		sendbuf[strlen(sendbuf)-1] = '\0';
		struct request rq = getRequest(sendbuf);
		handleRespond(socket_fd, rq);
		//logout
		if (rq.command == LOGOUT)
			break;
		
	}
	close(socket_fd);
	exit(0);
}
