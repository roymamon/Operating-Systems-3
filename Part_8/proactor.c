#include "proactor.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
    int sockfd;
    proactorFunc func;
} proactor_args_t;

void *proactor_main(void *arg) {
    proactor_args_t *args = (proactor_args_t *)arg;
    int listen_fd = args->sockfd;
    proactorFunc handler = args->func;
    free(arg);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        int *fd_ptr = malloc(sizeof(int));
        *fd_ptr = client_fd;

        pthread_t t;
        pthread_create(&t, NULL, (void *(*)(void *))handler, fd_ptr);
        pthread_detach(t);
    }

    return NULL;
}

pthread_t startProactor(int sockfd, proactorFunc threadFunc) {
    proactor_args_t *args = malloc(sizeof(proactor_args_t));
    args->sockfd = sockfd;
    args->func = threadFunc;

    pthread_t tid;
    pthread_create(&tid, NULL, proactor_main, args);
    return tid;
}

int stopProactor(pthread_t tid) {
    // For assignment purposes, assume the proactor runs forever.
    return pthread_cancel(tid);
}