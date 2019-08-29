#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_PORT 8088 // 端口
#define MAX_LISTEN 5 // 最大监听数
#define MAX_BUF 1024 // 缓冲区大小

struct pthread_data{
    struct sockaddr_in client_addr;
    int my_fd;
};

void ErrorHandling(char* message); // 错误处理函数
void* ServerForClient(void *arg);

// 主函数
int main(int argc, char *argv[]){
    int sockfd, new_fd;
    socklen_t len = sizeof(struct sockaddr);
    struct sockaddr_in sever_addr, client_addr;
    struct pthread_data pdata;

    bzero(&sever_addr, sizeof(sever_addr) );
    sever_addr.sin_family = AF_INET;
    sever_addr.sin_port = htons(SERVER_PORT);
    sever_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 采用TCP协议
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        ErrorHandling("socket error");
    
    // 绑定
    if(bind(sockfd, (struct sockaddr*)&sever_addr, sizeof(struct sockaddr)) == -1)
        ErrorHandling("bind error");

    // 监听
    if(listen(sockfd, MAX_LISTEN) == -1)
        ErrorHandling("listen error");
    
    // 服务器开启服务
    puts("service open");
    while(1){
        if((new_fd = accept(sockfd, (struct sockaddr*)&client_addr, &len)) == -1){
            perror("accept error!");
            continue;
        }
        else
            puts("new client connected...");

        // 创建新线程
        pthread_t* pt = (pthread_t*)malloc(sizeof(pthread_t));
        pdata.client_addr = client_addr;
        pdata.my_fd = new_fd;
        pthread_create(pt, NULL, ServerForClient, (void *)&pdata);
    }

    // 关闭服务器
    close(new_fd);
    close(sockfd);
    return 0;
}

void ErrorHandling(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

void* ServerForClient(void *arg){
    struct pthread_data* pdata = (struct pthread_data*)arg;
    int client_fd = pdata->my_fd;
    socklen_t len;
    char buf[MAX_BUF + 1];
    while(1){
        bzero(buf, MAX_BUF + 1);
        len = recv(client_fd, buf, MAX_BUF, 0);
        if(strcmp(buf, "quit")==0)
            break;
        if(len > 0)
            send(client_fd, buf, len, 0);
    }
}
