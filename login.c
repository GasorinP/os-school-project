#include "server.h"

char** LoadUserList()
{
	char** userlist = (char**)malloc(sizeof(char*)*6);
	for (int i=0;i<6;i++){
		userlist[i] = (char*)malloc(sizeof(char)*10);
	}
	strcpy(userlist[0],"user1");
	strcpy(userlist[1],"user2");
	strcpy(userlist[2],"user3");
	strcpy(userlist[3],"user4");
	strcpy(userlist[4],"user5");
	strcpy(userlist[5],"user6");
	return userlist;
}

int CheckLogin(char** userlist, char* buf, int n)
{
	int ISUSER = -1;
	for (int i=0;i<6;i++){
		if( strncmp(userlist[i], buf, n-1) == 0)
			ISUSER = i;
	}
	return ISUSER;
}

int getUserGroup(int userid)
{
	if (userid >= 0 && userid < 3) return GROUP1;
	else if(userid >= 3 && userid < 6) return GROUP2;
	return -1;
}