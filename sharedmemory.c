#include "server.h"

struct Object* getObjectArray(int proc) //porc = 0 : parent , proc = 1 : child
{
    int flags;
    if (proc == PARENT) flags = O_CREAT | O_RDWR | O_TRUNC;
    else if(proc == CHILD) flags = O_RDWR;
    else flags = 0;

    int shared_fd = shm_open("ObjectArray", flags, S_IRUSR | S_IWUSR); //set shared memory fd
	if (shared_fd == -1){
		perror("shm_open");
		exit(1);
	}
	if (ftruncate(shared_fd, sizeof(struct Object)*OBJECT_MAX_NUMBER)){ //set size of shared memory = objectarray[1024]
		perror("ftruncate");
		exit(1);
	}
	struct Object* ObjectArray = (struct Object*)mmap(NULL, sizeof(struct Object)*OBJECT_MAX_NUMBER, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);
	if (ObjectArray == MAP_FILE){
		perror("mmap");
		exit(1);
	}
    return ObjectArray;
}

int* getNumObject(int proc)
{
    int flags;
    if (proc == PARENT) flags = O_CREAT | O_RDWR | O_TRUNC;
    else if(proc == CHILD) flags = O_RDWR;
    else flags = 0;
    int shared_fd = shm_open("NumObject", flags, S_IRUSR | S_IWUSR); //set shared memory fd
    if (shared_fd == -1){
		perror("shm_open");
		exit(1);
	}
    if (ftruncate(shared_fd, sizeof(int))){ //set size of shared memory = objectarray[1024]
		perror("ftruncate");
		exit(1);
	}
    int* NumObject = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);
	if (NumObject == MAP_FILE){
		perror("mmap");
		exit(1);
	}
    return NumObject;
}

