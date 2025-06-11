CC=gcc
target1=server
target2=client
src=$(wildcard *.c)
deps=$(wildcard *.h)
obj=$(patsubst %.c,%.o,$(src))

all:$(target1) $(target2)

$(target1) : server.o login.o handlerequest.o sharedmemory.o capabilitylist.o checkpermission.o
	$(CC) $^ -o $@ -Wall -lrt

$(target2) : client.o handlerespond.o
	$(CC) $^ -o $@ -Wall
	
%.o : %.c $(deps)
	$(CC) -c $< -o $@ -Wall
	
.PHONY : clean
clean :
	-rm -rf $(target1) $(target2) $(obj)
