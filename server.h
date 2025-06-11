/* server.h : function prototypes for all functions in server exercise */
#include "common.h"

#define SERVER_FILE_ROOT "./serverdisk/"
#define OBJECT_MAX_NUMBER 1024
#define SHAREDFILE "./ObjectArray"
#define PARENT 0
#define CHILD 1
#define GROUP1 0
#define GROUP2 1

struct UserInfo{
    int userid;
    int group;
};

struct Object{
    struct UserInfo owner;
    char objectname[FILE_NAME_SIZE];
    int permission;
};

struct ObjectArray{
    struct Object* array;
    int* num;
};

struct AccessRight{
    char objectname[FILE_NAME_SIZE];
    int rightset;
};

struct CapabilityNode{
    struct AccessRight AccessRight;
    struct CapabilityNode* next;
};
//login
char** LoadUserList();
int CheckLogin(char** userlist, char* buf, int n);
int getUserGroup(int userid);
//getRequest
struct request getRequest(char* buf);
//handleRequest
int createFile(struct ObjectArray ObjectArray, struct UserInfo userinfo, struct request rq);
int writeFile(struct CapabilityNode* CapabilityList, int connect_fd, struct request rq);
int readFile(struct CapabilityNode* CapabilityList,int connect_fd, struct request rq);
void handleRequest(struct ObjectArray ObjectArray, struct CapabilityNode* CapabilityList, struct UserInfo userinfo, int connect_fd, struct request rq);
//shared memory
struct Object* getObjectArray();
int* getNumObject(int proc);
//CapabilityList
struct CapabilityNode* getCapabilityList(struct ObjectArray ObjectArray, struct UserInfo UserInfo);
int getRightset(struct ObjectArray ObjectArray, struct UserInfo UserInfo, int i);
//checkPermission
int checkPermission(struct CapabilityNode* CapabilityList, struct request rq);
int searchCapabilityList(struct CapabilityNode* CapabilityList, struct request rq, int need);

