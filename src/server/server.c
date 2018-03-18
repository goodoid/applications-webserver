/**
 * Author            : wangguo <wangguo@didichuxing.com>
 * Date              : 18.03.2018
 * Last Modified Date: 18.03.2018
 */
#include <socket.h>
#include <stdio.h>

void server_handler(int conn_fd){
}

int start_server(int port){
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd <= 0){
        printf("new socket failed:%d\n", socket_fd);
        return -1;
    }
    struct sockaddr addr;
    addr.sa_family = AF_INET;
    addr.sa_data = itoa(port);
    int ret = bind(socket_fd, &addr, sizeof(addr));
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
        int recv_fd = accept(socket_fd, &addr, sizeof(addr));
        pthread_create(recv_fd);
    }
}

int main(int argc, char **argv){
    if(argc < 1){
        printf("Usage:%s [server port]\n");
        return -1;
    }
}
