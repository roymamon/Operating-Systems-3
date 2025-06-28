#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include "proactor.h"
#include <stdatomic.h>

extern atomic_int thread_counter;

typedef struct {
    int sockfd;
    proactorFunc func;
} ProactorContext;

void *accept_loop(void *arg) {
    ProactorContext *ctx = (ProactorContext *)arg;

    int thread_id = atomic_fetch_add(&thread_counter, 1);
    printf("Thread %d: Accept thread started\n", thread_id);

    while (1) {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int client_fd = accept(ctx->sockfd, (struct sockaddr*)&client, &len);
        if (client_fd < 0) continue;

        int *pclient = malloc(sizeof(int));
        *pclient = client_fd;

        pthread_t tid;
        pthread_create(&tid, NULL, (void *(*)(void *))ctx->func, pclient);
        pthread_detach(tid);
    }
    return NULL;
}

pthread_t startProactor(int sockfd, proactorFunc threadFunc) {
    ProactorContext *ctx = malloc(sizeof(ProactorContext));
    ctx->sockfd = sockfd;
    ctx->func = threadFunc;

    pthread_t tid;
    pthread_create(&tid, NULL, accept_loop, ctx);
    return tid;
}

int stopProactor(pthread_t tid) {
    return pthread_cancel(tid);
}