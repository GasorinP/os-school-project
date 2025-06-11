#include "server.h"

int getRightset(struct ObjectArray ObjectArray, struct UserInfo UserInfo, int i){
    int permission = (ObjectArray.array[i].permission);
    int rightset = 0;
    
    rightset = rightset + (int)(permission/100) * 100;
    permission = permission - rightset;

    if(ObjectArray.array[i].owner.userid == UserInfo.userid){
        rightset = (int)(rightset/100) + 4;
        return rightset; 
    }
    
    rightset= (int)(permission/10) * 10;
    permission = permission - rightset;

    if(ObjectArray.array[i].owner.group == UserInfo.group){
        return (int)(rightset/10);
    }
    
    rightset  = permission; 
    return rightset;
}

struct CapabilityNode* getCapabilityList(struct ObjectArray ObjectArray, struct UserInfo UserInfo)
{
    struct CapabilityNode* front = NULL;
    struct CapabilityNode* end = NULL;
    int numObject = *(ObjectArray.num);
    int rightset;
    for(int i=0;i<numObject;i++){
        rightset = getRightset(ObjectArray, UserInfo, i);
        if(rightset != 0){
            struct CapabilityNode* temp = (struct CapabilityNode*)malloc(sizeof(struct CapabilityNode));
            strcpy(temp->AccessRight.objectname, ObjectArray.array[i].objectname);
            temp->AccessRight.rightset = rightset;
            temp->next = NULL;
            if (front == NULL){
                front = temp;
                end = front;
            }
            else{
                end->next = temp;
                end = temp;
            }
        }
    }
    return front;
}

