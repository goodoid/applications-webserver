/**
 * Author            : wangguo <wangguo@didichuxing.com>
 * Date              : 18.03.2018
 * Last Modified Date: 19.03.2018
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>

void* server_handler(void* req){
    int* ret = new int(0);
    int recv_fd = *((int*)req);
    char buffer[1024];
    int read_len = read(recv_fd, buffer, sizeof(buffer));
    if (read_len < 1) {
        *ret = -1;
        printf("received len:%d content:%s\n", read_len, buffer);
        return ret;
    }
    return ret; //TODO how ensure exit without pthread_join
}
int handle_req(int  recv_fd){
    pthread_t thrd;
    //struct pthread_attr_t attr;
    int ret = pthread_create(&thrd, NULL, server_handler, (void*)recv_fd);
    if (0 != ret){
        printf("new thread failed ret:%d",ret);
        return -1;
    }
    return 0;
}
int start_server(int port){
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd <= 0){
        printf("new socket failed:%d\n", socket_fd);
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    int ret = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret != 0) {
        printf("bind failed:%d\n", ret);
        return -2;
    }
    ret=listen(socket_fd, 0);
    if(ret!=0){
        printf("listen failed:%d\n", ret);
        return -3;
    }
    while(true) {
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        int recv_fd = accept(socket_fd, (struct sockaddr*)&peer_addr, &peer_addr_len);//NOTE will block, how to nonblock?
        if (-1 == recv_fd){
            ret = handle_req(socket_fd);
            if (ret != 0){
                printf("handler request failed for socket_fd:%d ret:%d\n", socket_fd, ret);
                return -1;
            }
        }
    }
    return 0;
}
int main(int argc, char **argv){
    if(argc < 1){
        printf("Usage:%s [server port]\n");
        return -1;
    }
    int port = atoi(argv[1]);
    return start_server(port);
}
