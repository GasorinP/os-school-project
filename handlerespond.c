#include "client.h"

struct request getRequest(char* buf)
{
	const char delim[2] = " ";
	char *token;
	struct request rq;
	
	token = strtok(buf, delim);
	if(token != NULL)
	{
		if(strncmp(token, "create", strlen(token)) == 0){
			rq.command = CREATE;
			strcpy(rq.filename, "");
			rq.parameter = -1;
		}
		else if(strncmp(token, "read", strlen(token)) == 0){
			rq.command = READ;
			token = strtok(NULL, delim);
			strcpy(rq.filename, token);
			rq.filename[strlen(token)+1] = '\0';
			rq.parameter = -1;
		}
		else if(strncmp(token, "write", strlen(token)) == 0){
			rq.command = WRITE;
			strcpy(rq.filename, "");
			rq.parameter = -1;
		}
		else if(strncmp(token, "logout", strlen(token)) == 0){
			rq.command = LOGOUT;
			strcpy(rq.filename, "");
			rq.parameter = -1;
		}
		else if(strncmp(token, "changemode", strlen(token)) == 0){
			rq.command = CHANGEMODE;
			strcpy(rq.filename, "");
			rq.parameter = -1;
		}
		else{
			rq.command = -2;
			strcpy(rq.filename, "");
			rq.parameter = -1;
		}
	}
	return rq;	
}

void handleRespond(int socket_fd, struct request rq)
{
	int fd, numRecv, numWrite;
	char sendbuf[BUF_SIZE];
	char recvbuf[BUF_SIZE];
	memset(recvbuf, 0, BUF_SIZE);
	char path[FILE_NAME_SIZE];
	strcpy(path,CLIENT_FILE_ROOT);
	strcat(path, rq.filename);
	
	if(rq.command == CREATE){
		
		numRecv = recv(socket_fd, recvbuf, BUF_SIZE, 0);
		if( (numRecv) == -1){
			perror("recv");
			return;
		}
		recvbuf[numRecv] = '\0';
		printf("Received : %s\n", recvbuf);
	}
	
	if(rq.command == READ){
		
		if ((numRecv = recv(socket_fd, recvbuf, BUF_SIZE, 0)) == -1){
			perror("recverror");
			return;
		}
		
		if(strncmp(recvbuf, "Read Error", strlen("Read Error")-1) == 0){
			printf("Received : %s\n",recvbuf);
			return;
		}
		else{
			printf("Received : Read Success\n");
			printf("File : %s\n",recvbuf);
		}
		if( (fd = open(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
			perror("openerror");
			return;
		}
		if((numWrite = write(fd, recvbuf, numRecv)) == -1){
			perror("writeerror");
			return;
		}
	}
	
	if(rq.command == WRITE){
		char buf[BUF_SIZE];
		memset(sendbuf, 0, BUF_SIZE);
		//write data
		printf("=====write=====\n");
		fgets(buf, BUF_SIZE, stdin);
		while( strncmp(buf, ":wq", 3) != 0){
			strcat(sendbuf, buf);
			fgets(buf, BUF_SIZE, stdin);
		}
		printf("=====end=====\n");
		//send data
		if( send(socket_fd, sendbuf, strlen(sendbuf), 0) == -1){
			printf("send msg error: %s(errno: %d)\n",strerror(errno), errno);
			exit(0);
		}
		if( (numRecv = recv(socket_fd, recvbuf, BUF_SIZE, 0)) == -1){
			perror("recv error");
			exit(0);	
		}
		recvbuf[numRecv] = '\0';
		printf("Received : %s\n", recvbuf);
	}

	if(rq.command == CHANGEMODE){
		
		numRecv = recv(socket_fd, recvbuf, BUF_SIZE, 0);
		if( (numRecv) == -1){
			perror("recv");
			return;
		}
		recvbuf[numRecv] = '\0';
		printf("Received : %s\n", recvbuf);
	}
}






























