/**
 * Author            : wangguo
 * Date              : 23.03.2018
 * Last Modified Date: 02.04.2018
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>

void* server_handler(void* req){
    int* ret = new int(0);
    int recv_fd = *((int*)req);
    std::cout<<"handle recv_fd:"<<recv_fd<<std::endl;
    char buffer[1024];
    int read_ret = read(recv_fd, buffer, sizeof(buffer));
    int read_len = read_ret;
    //while(read_ret > 0){
    //    read_ret = read(recv_fd, buffer+read_len, sizeof(buffer)-read_len);
    //    if (read_ret <= 0){
    //        std::cout<<"read empty"<<std::endl;
    //        break;
    //    }
    //    read_len+=read_ret;
    //    std::cout<<"read recv_fd:"<<recv_fd<<"len:"<<read_ret<<"total:"<<read_len<<std::endl;
    //}
    //int http_handler(req_buffer, req_len, res_buffer, res_len);
    std::cout<<"received len:"<<read_len<<"content:\n{"<<buffer<<"}\n"<<std::endl;
    char res_buffer[1024];
    char content_buffer[2048];
    int content_len = snprintf(content_buffer, sizeof(content_buffer), "hello from goodriod\n");
    int res_len = snprintf(res_buffer, sizeof(res_buffer), "HTTP/1.1 200 OK\nServer: Apache\nVary: Accept-Encoding\nContent-Length: %d\n\n%s", content_len, content_buffer);
    int write_len = write(recv_fd,res_buffer,res_len);
    if (write_len != res_len){
        printf("write recv_fd:%d failed\n", recv_fd);
        *ret = -1;
        return ret;
    }
    close(recv_fd);
    std::cout<<"write len:"<<write_len<<"content:{"<<res_buffer<<"}"<<std::endl;
    return ret; //TODO how ensure exit without pthread_join
}
int handle_req(int recv_fd){
    pthread_t thrd;
    //struct pthread_attr_t attr;
    int* arg = new int(recv_fd);//TODO delete
    int ret = pthread_create(&thrd, NULL, server_handler, (void*)arg);
    if (0 != ret){
        delete arg;
        printf("new thread failed ret:%d\n",ret);
        return -1;
    }
    //delete arg;
    //printf("join thread:%d\n", thrd);
    //pthread_join(thrd, NULL);
    return 0;
}
int start_server(int port){
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd <= 0){
        printf("new socket failed:%d\n", socket_fd);
        return -1;
    }
    struct linger linger_opt;
    linger_opt.l_onoff = 1;
    linger_opt.l_linger = 3;
    int socket_opt_reuseport = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_LINGER,(void*)&linger_opt, sizeof(linger_opt));
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, (void*)&socket_opt_reuseport, sizeof(socket_opt_reuseport));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    int ret = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret != 0) {
        printf("bind failed:%d\n", ret);
        return -2;
    }
    printf("bind port:%d ok\n", port);
    ret=listen(socket_fd, 0);
    if(ret!=0){
        printf("listen failed:%d\n", ret);
        return -3;
    }
    printf("listen ok. fd:%d\n", socket_fd);
    int conn_count = 0;
    while(true) {
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        printf("accepting\n");
        int recv_fd = accept(socket_fd, (struct sockaddr*)&peer_addr, &peer_addr_len);//NOTE will block, how to nonblock?
        printf("accepted id:%d recv_fd:%d\n", conn_count++, recv_fd);
        if (-1 != recv_fd){
            ret = handle_req(recv_fd);
            if (ret != 0){
                printf("handler request failed for socket_fd:%d ret:%d\n", socket_fd, ret);
                return -1;
            }
            std::cout<<"handle done recv_fd:"<<recv_fd<<std::endl;
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
