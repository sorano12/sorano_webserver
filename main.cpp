#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<unistd.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>

#include "processpoll.h"
#include "echo.h"

int main(int argc,char* argv[]){
    if(argc<=2){
        printf("usage: %s ip_address portname\n",argv[0]);
        return 0;
    }

    const char* ip=argv[1];
    int port=atoi(argv[2]);

    int listenfd=socket(PF_INET,SOCK_STREAM,0);
    assert(listenfd>=1);

    struct sockaddr_in address;
    memset(&address,0,sizeof(address));

    int ret=0;
    ret=bind(listenfd,(struct sockaddr*)(&address),sizeof(address));
    assert(ret!=-1);

    processpool<echo>* pool=processpool<echo>::create(listenfd,8);
    pool->run();

    close(listenfd);

    return 0;
}
