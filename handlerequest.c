#include "server.h"

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
			token = strtok(NULL, delim);
			strcpy(rq.filename, token);
			rq.filename[strlen(token)] = '\0';
			token = strtok(NULL, delim);
			int pm = 0;
			if (token[0] == 'r') pm = pm + 2*100;
			if (token[1] == 'w') pm = pm + 1*100;
			if (token[2] == 'r') pm = pm + 2*10;
			if (token[3] == 'w') pm = pm + 1*10;
			if (token[4] == 'r') pm = pm + 2;
			if (token[5] == 'w') pm = pm + 1;
			rq.parameter = pm;
		}
		else if(strncmp(token, "read", strlen(token)) == 0){
			rq.command = READ;
			token = strtok(NULL, delim);
			strcpy(rq.filename, token);
			rq.filename[strlen(token)] = '\0';
			rq.parameter = -1;
		}
		else if(strncmp(token, "write", strlen(token)) == 0){
			rq.command = WRITE;
			token = strtok(NULL, delim);
			strcpy(rq.filename, token);
			rq.filename[strlen(token)] = '\0';
			token = strtok(NULL, delim);
			int pm = 0;
			if (token[0] == 'o') pm = 0;
			if (token[0] == 'a') pm = 1;
			rq.parameter = pm;
		}
		else if(strncmp(token, "changemode", strlen(token)) == 0){
			rq.command = CHANGEMODE;
			token = strtok(NULL, delim);
			strcpy(rq.filename, token);
			rq.filename[strlen(token)] = '\0';
			token = strtok(NULL, delim);
			int pm = 0;
			if (token[0] == 'r') pm = pm + 2*100;
			if (token[1] == 'w') pm = pm + 1*100;
			if (token[2] == 'r') pm = pm + 2*10;
			if (token[3] == 'w') pm = pm + 1*10;
			if (token[4] == 'r') pm = pm + 2;
			if (token[5] == 'w') pm = pm + 1;
			rq.parameter = pm;
		}
		else if(strncmp(token, "logout", strlen(token)) == 0){
			rq.command = LOGOUT;
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

int createFile(struct ObjectArray ObjectArray, struct UserInfo userinfo, struct request rq)
{
	sleep(1);
	int fd; //file fd
	pid_t ppid = getppid(); //parent pid
	char path[FILE_NAME_SIZE]; //file path
	//set file path
	strcpy(path,SERVER_FILE_ROOT);
	strcat(path, rq.filename);
	//create file
	if ((fd = open(path, O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1){
		perror("open");
		return -1;
	}
	//add object to ObjectArray
	int ptr = *(ObjectArray.num);
	ObjectArray.array[ptr].owner.userid = userinfo.userid;
	ObjectArray.array[ptr].owner.group = userinfo.group;
	strncpy(ObjectArray.array[ptr].objectname, rq.filename, strlen(rq.filename));
	ObjectArray.array[ptr].permission = rq.parameter;
	ptr = ptr + 1;
	*(ObjectArray.num) = ptr;
	//send signal to server
	kill(ppid, SIGUSR1);

	return fd;
}

int readFile(struct CapabilityNode* CapabilityList, int connect_fd, struct request rq)
{
	sleep(3);
	int fd, numRead;
	char buf[BUF_SIZE];
	char path[FILE_NAME_SIZE];
	strcpy(path,SERVER_FILE_ROOT);
	strcat(path, rq.filename);
	if (access(path, F_OK) == -1){
		return -1;
	}
	if (checkPermission(CapabilityList, rq) == -1){
		return -2;
	}
	if ((fd = open(path, O_RDONLY)) == -1){
		perror("open");
		return -1;
	}
	while((numRead = read(fd, buf, BUF_SIZE)) > 0){
		if ( send(connect_fd, buf, numRead, 0) == -1){
			perror("senderror");
			close(fd);
			return -1;	
		}
	}
	if (numRead == -1){
		perror("readerror");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int writeFile(struct CapabilityNode* CapabilityList, int connect_fd, struct request rq)
{
	sleep(3);
	int fd, numWrite, numRead;
	char buf[BUF_SIZE];
	char path[FILE_NAME_SIZE];
	strcpy(path,SERVER_FILE_ROOT);
	strcat(path, rq.filename);
	int flags;
	if (rq.parameter == 0) flags = O_WRONLY | O_TRUNC;
	else if (rq.parameter == 1) flags = O_WRONLY | O_APPEND;
			
	if ((numRead = recv(connect_fd, buf, BUF_SIZE, 0)) > 0){
	
		if ((fd = open(path, flags)) == -1){
			perror("open");
			return -1;
		}
		if (checkPermission(CapabilityList, rq) == -1){
			return -2;
		}
		if ((numWrite = write(fd, buf, numRead)) == -1){
			perror("write");
			close(fd);
			return -1;
		}
	}
	else{
		close(fd);
		perror("recv");
		return -1;
	}
	close(fd);
	return 0;
}

int changemodeFile(struct ObjectArray ObjectArray, struct CapabilityNode* CapabilityList, struct UserInfo userinfo, struct request rq)
{
	sleep(1);
	int numObject = *(ObjectArray.num);
	for(int i=0;i<numObject;i++){
		if(strcmp(ObjectArray.array[i].objectname, rq.filename) == 0){
			if (checkPermission(CapabilityList, rq) == -1){
				return -2;
			}
			ObjectArray.array[i].permission = rq.parameter;
			return 0;
		}
	} 
	return -1;
}

void handleRequest(struct ObjectArray ObjectArray, struct CapabilityNode* CapabilityList, struct UserInfo userinfo, int connect_fd, struct request rq)
{	
	int fd, numRead, numWrite;
	char respond[BUF_SIZE];
	
	if (rq.command == CREATE){	
		if((fd = createFile(ObjectArray, userinfo, rq)) == -1)
			strcpy(respond, "Creat Error");
		else
			strcpy(respond, "Create Success");
		if ( send(connect_fd, respond, strlen(respond), 0) == -1)
			perror("senderror");
		close(fd);
	}
	
	if (rq.command == READ){
		int error = readFile(CapabilityList, connect_fd, rq);
		if(error == -1){
			strcpy(respond, "Read Error(Read File Error)");
			if ( send(connect_fd, respond, strlen(respond), 0) == -1)
				perror("senderror");
		}
		else if(error == -2){
			strcpy(respond, "Read Error(Permission Denied)");
			if ( send(connect_fd, respond, strlen(respond), 0) == -1)
				perror("senderror");
		}
	}
	
	if (rq.command == WRITE){
		int error = writeFile(CapabilityList, connect_fd, rq);
		if(error == -1){
			strcpy(respond, "Write Error(Write File Error)");
		}
		else if(error == -2){
			strcpy(respond, "Write Error(Permission Denied)");
		}
		else
			strcpy(respond, "Write Success");
		if ( send(connect_fd, respond, strlen(respond), 0) == -1)
			perror("senderror");
	}

	if (rq.command == CHANGEMODE){
		fd = changemodeFile(ObjectArray, CapabilityList, userinfo, rq);
		if( fd == -1)
			strcpy(respond, "ChangeMode Error(File dosn's exist)");
		else if(fd == -2)
			strcpy(respond, "ChangeMode Error(Permission Denied)");
		else
			strcpy(respond, "ChangeMode Success");
		if ( send(connect_fd, respond, strlen(respond), 0) == -1)
			perror("senderror");
		close(fd);
	}
}



















