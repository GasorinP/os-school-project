/* client.h : function prototypes for all functions in client exercise */
#include "common.h"

#define CLIENT_FILE_ROOT "./clientdisk/"

struct request getRequest(char* buf);
void handleRespond(int socket_fd, struct request rq);
