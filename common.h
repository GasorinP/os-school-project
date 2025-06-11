#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <semaphore.h>


#define DEFAULT_PORT 8000
#define BUF_SIZE 4096
#define FILE_NAME_SIZE 1024

#define CREATE 0
#define READ 1
#define WRITE 2
#define CHANGEMODE 3
#define LOGOUT -1

struct request
{
	int command;
	char filename[FILE_NAME_SIZE];
	int parameter;
};

