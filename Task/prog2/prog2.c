#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <dlfcn.h>

#define SOCKET_PATH "/tmp/my_socket"
#define BACKLOG 5

char * (*analyze_string)(int);

int main() {
    int sockfd, newsockfd, clilen;
    struct sockaddr_un serv_addr, cli_addr;
    int n;

    unlink(SOCKET_PATH);

    if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        perror("Error opening socket");
        exit(1);
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_LOCAL;
    strncpy(serv_addr.sun_path, SOCKET_PATH, sizeof(serv_addr.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error on binding");
        exit(1);
    }

    listen(sockfd, BACKLOG);
    clilen = sizeof(cli_addr);

    while (1) {
        if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
            perror("Error on accept");
            continue;
        }

        int result;
        n = recv(newsockfd, &result, sizeof(result), 0);
        if (n < 0) {
            perror("Error reading from socket");
            close(newsockfd);
            continue; 
        }
        printf("Program 2 received result: %d\n", result);
        void * handle = dlopen("./libmylib.so", RTLD_LAZY);
        if (handle ==NULL){
            perror("dlopen() error");
            exit(1);
        }
        analyze_string = dlsym(handle, "analyze_string");
        if (dlerror()) {
            perror("dlsym() error\n");
            dlclose(handle);
            return -1;
        }
        char *analyze_res = analyze_string(result);
        printf("Результат: %s\n", analyze_res);
        dlclose(handle);
        close(newsockfd);
    }
    close(sockfd);
    return 0;
}
