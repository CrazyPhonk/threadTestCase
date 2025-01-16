#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <semaphore.h>
#include <errno.h>
#include <dlfcn.h>

void (*sort_and_replace)(char *, char *, size_t);
int (*sum_numeric)(char *);
char * (*analyze_string)(char *);

#define BUFFER_SIZE 65
#define SOCKET_PATH "/tmp/my_socket"
char buffer[BUFFER_SIZE];
sem_t buffer_empty;
sem_t buffer_full;
pthread_mutex_t buffer_mutex;

void *thread1_function(void *arg);
void *thread2_function(void *arg);


void *thread1_function(void *arg) {
    char input[BUFFER_SIZE];
    size_t res_size = 256;
    char res[res_size];
    while (1) {
        printf("Enter a string (up to 64 digits), use spaces: ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;

        int len = strlen(input);
        if (len == 0 || len > 64) {
            printf("Error: String length must be between 1 and 64 characters.\n");
            continue;
        }
        for (int i = 0; i < len; i++) {
            if (!isdigit(input[i]) && input[i] != ' ') {
                printf("Error: String must contain only digits and spaces.\n");
                continue;
            }
        }
        sem_wait(&buffer_empty);
        pthread_mutex_lock(&buffer_mutex);
        void * handle = dlopen("./libmylib.so", RTLD_LAZY);
        if (handle ==NULL){
            perror("dlopen() error");
            exit(1);
        }
        sort_and_replace = dlsym(handle, "sort_and_replace");
        if (dlerror()) {
            perror("dlsym() error\n");
            dlclose(handle);
            return NULL;
        }
        sort_and_replace(input, res, res_size);
        dlclose(handle);
        strcpy(buffer, res);
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&buffer_full);
    }
    return NULL;
}

void *thread2_function(void *arg) {
    struct sockaddr_un serv_addr;
    int sockfd, n;

    while (1) {
        sem_wait(&buffer_full);
        pthread_mutex_lock(&buffer_mutex);
        printf("Thread 2 received: %s\n", buffer);
        printf("\n");
        void * handle = dlopen("./libmylib.so", RTLD_LAZY);
        if (handle ==NULL){
            perror("dlopen() error");
            exit(1);
        }
        sum_numeric = dlsym(handle, "sum_numeric");
        if (dlerror()) {
            perror("dlsym() error\n");
            dlclose(handle);
            return NULL;
        }
        int result = sum_numeric(buffer);
        dlclose(handle);
        if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            pthread_mutex_unlock(&buffer_mutex);
            sem_post(&buffer_empty);
            continue;
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sun_family = AF_LOCAL;
        strncpy(serv_addr.sun_path, SOCKET_PATH, sizeof(serv_addr.sun_path) - 1);

        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection Failed");
            close(sockfd);
            pthread_mutex_unlock(&buffer_mutex);
            sem_post(&buffer_empty);
            continue;
        }

        n = send(sockfd, &result, sizeof(result), 0);
        if (n < 0) perror("Send failed");
        close(sockfd);
        buffer[0] = '\0';
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&buffer_empty);
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    sem_init(&buffer_empty, 0, 1);
    sem_init(&buffer_full, 0, 0);
    pthread_mutex_init(&buffer_mutex, NULL);

    pthread_create(&thread1, NULL, &thread1_function, NULL);
    pthread_create(&thread2, NULL, &thread2_function, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    sem_destroy(&buffer_empty);
    sem_destroy(&buffer_full);
    pthread_mutex_destroy(&buffer_mutex);
}