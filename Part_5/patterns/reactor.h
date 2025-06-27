#ifndef REACTOR_H
#define REACTOR_H

typedef void (*reactorFunc)(int fd);

void runReactor(void *reactor);
void *startReactor();
int addFdToReactor(void *reactor, int fd, reactorFunc func);
int removeFdFromReactor(void *reactor, int fd);
int stopReactor(void *reactor);

#endif