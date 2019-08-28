#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "process.h"

const unsigned int port = 8088; // 端口
const unsigned int lisnum = 5; // 最大监听数

void ErrorHandling(char* message); // 错误处理函数

// 主函数
int main(int argc, char *argv[]){
    int sockfd, new_fd;
    socklen_t len;
    struct sockaddr_in sever_addr, client_addr;

    bzero(&sever_addr, sizeof(sever_addr) );
    sever_addr.sin_family = AF_INET;
    sever_addr.sin_port = htons(port);
    sever_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 采用TCP协议
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        ErrorHandling("socket error");
    
    // 绑定
    if(bind(sockfd, (struct sockaddr*)&sever_addr, sizeof(struct sockaddr)) == -1)
        ErrorHandling("bind error");

    // 监听
    if(listen(sockfd, lisnum) == -1)
        ErrorHandling("listen error");
    
    // 服务器开启服务
    puts("service open");
    while(1){
        len = sizeof(struct sockaddr);
        if((new_fd = accept(sockfd, (struct sockaddr*)&client_addr, &len)) == -1){
            perror("accept error!");
            continue;
        }
        else
            puts("new client connected...");
        // 创建新进程
        switch(fork()){
            case -1:
                close(new_fd);
                break;
            case 0:
                Process(new_fd);
                break;
        }
    }

    // 关闭服务器
    close(sockfd);
    return 0;
}

void ErrorHandling(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}
