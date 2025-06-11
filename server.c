#include "server.h"

int objectChanged = 0;
int turnoff = 0;
void sigroutine(int dunno)
{
	objectChanged = 1;
	//printf("objectChanged = %d\n",objectChanged);
}
void sigturnoff(int dunno)
{
	turnoff = 1;
}
int main(int argc, char* argv[])
{
	int socket_fd, connect_fd;
	struct sockaddr_in servaddr;
	char buf[BUF_SIZE];
	int numRead;
	//=====initialize server=====

	//load user list
	char** userlist = LoadUserList();
	printf("load userlist success\n");

	//initialize shared memory
	struct ObjectArray ObjectArray;
	ObjectArray.array = getObjectArray(PARENT);
	ObjectArray.num = getNumObject(PARENT);
	*(ObjectArray.num) = 0;
	printf("mmap %s success\n",SHAREDFILE);

	//set signal handler
	sigset_t newmask;
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	sigprocmask(SIG_UNBLOCK, &newmask, NULL);
	signal(SIGUSR1, sigroutine);
	signal(SIGINT, sigturnoff);

	//initialize socket
	if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("creat socket error: %s(errno: %d)\n",strerror(errno), errno);
		exit(0);
	}
	printf("set signal handler success\n");

	//initialize servaddr
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//set IP addr => INADDR_ANY
	servaddr.sin_port = htons(DEFAULT_PORT);

	//set ipv4
	if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		printf("bind socket error: %s(errno: %d)\n",strerror(errno), errno);
		exit(0);
	}

	//listen client connect
	if( listen(socket_fd, 10) == -1){
		printf("listen socket error: %s(errno: %d)\n",strerror(errno), errno);
		exit(0);
	}

	//=====initialize server=====

	printf("======server is working======\n");
	while(1){
		if(turnoff == 1){
			close(socket_fd);
			exit(0);
		}
		//block server until accept client's request
		if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){
			printf("accept socket error: %s(errno: %d)\n",strerror(errno), errno);
			continue;
		}
		//Login
		// read client's username
		numRead = recv(connect_fd, buf, BUF_SIZE, 0);
		
		//check login
		int userid = -1;
		if(numRead > 0){
			userid = CheckLogin(userlist, buf, numRead);
			if (userid == -1){
				if( send(connect_fd, "faile",5,0) == -1)
					perror("senderror");
				close(connect_fd);
				continue;
			}
			else{
				if( send(connect_fd, "success",7,0) == -1)
					perror("senderror");
			}
		}
		buf[numRead-1] = '\0';
		printf("Login : %s\n",buf);
		//fork child process to handle client's request
		if(!fork()){
			ObjectArray.array = getObjectArray(CHILD);
			ObjectArray.num = getNumObject(CHILD); 
			struct UserInfo userinfo;
			userinfo.userid = userid;
			userinfo.group = getUserGroup(userid);
			struct CapabilityNode* CapabilityList = NULL;
			while(1){
				
				//read client's request
				if( (numRead = recv(connect_fd, buf, BUF_SIZE, 0)) > 0){
					//get CapabilityList
					printf("=====%s CapabilityList=====\n",userlist[userinfo.userid]);
					CapabilityList = getCapabilityList(ObjectArray, userinfo);
					struct CapabilityNode* ptr = CapabilityList;
					while(ptr != NULL){
						printf("%s,%d\n",ptr->AccessRight.objectname,ptr->AccessRight.rightset);
						ptr = ptr->next;
					}
					printf("===========================\n");
					
					//print client's request
					buf[numRead-1] = '\0';
					char command[BUF_SIZE];
					strncpy(command, buf, numRead);
					struct request rq = getRequest(buf);
					printf("%s : %s\n",userlist[userinfo.userid], command);

					//handle request
					handleRequest(ObjectArray, CapabilityList, userinfo, connect_fd, rq);
					
					//logout
					if (rq.command == LOGOUT){
						printf("%s Logout Success\n", userlist[userinfo.userid]);
						break;
					}
					memset(command, 0, BUF_SIZE);
				}
			}	
			close(connect_fd);
			exit(0);
		}
		close(connect_fd);	
	}
	
}
