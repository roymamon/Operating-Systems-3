#ifndef PROACTOR_H
#define PROACTOR_H

#include <pthread.h>

typedef void *(*proactorFunc)(int *);

pthread_t startProactor(int sockfd, proactorFunc threadFunc);
int stopProactor(pthread_t tid);

#endif