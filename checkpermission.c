#include "server.h"

#define C 4
#define R 2
#define W 1

int searchCapabilityList(struct CapabilityNode* CapabilityList, struct request rq, int need){
    struct CapabilityNode* ptr = CapabilityList;
    while(ptr != NULL){
        
        if( strcmp(rq.filename, ptr->AccessRight.objectname) == 0){
            
            if ((ptr->AccessRight.rightset & need) == need){
                return 0;
            }
            else{
                return -1;
            }
        }
        ptr = ptr->next;
    }
    return -1;
}

int checkPermission(struct CapabilityNode* CapabilityList, struct request rq)
{
    if(rq.command == CREATE){
        return 0;
    }
    else if(rq.command == READ){
        return searchCapabilityList(CapabilityList, rq, R);
    }
    else if(rq.command == WRITE){
        return searchCapabilityList(CapabilityList, rq, W);
    }
    else if(rq.command == CHANGEMODE){
        return searchCapabilityList(CapabilityList, rq, C);
    }
    else if(rq.command == LOGOUT){
        return 0;
    }
    return -1;
}