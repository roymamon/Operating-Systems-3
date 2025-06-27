#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include "reactor.h"

#define MAX_FDS 1024

typedef struct {
    int active;
    reactorFunc func;
} FDHandler;

typedef struct {
    FDHandler handlers[MAX_FDS];
    int running;
} Reactor;

void *startReactor() {
    Reactor *r = malloc(sizeof(Reactor));
    if (!r) return NULL;
    memset(r, 0, sizeof(Reactor));
    r->running = 1;
    return r;
}

int addFdToReactor(void *reactor, int fd, reactorFunc func) {
    Reactor *r = (Reactor *)reactor;
    if (fd < 0 || fd >= MAX_FDS) return -1;
    r->handlers[fd].active = 1;
    r->handlers[fd].func = func;
    return 0;
}

int removeFdFromReactor(void *reactor, int fd) {
    Reactor *r = (Reactor *)reactor;
    if (fd < 0 || fd >= MAX_FDS) return -1;
    r->handlers[fd].active = 0;
    r->handlers[fd].func = NULL;
    return 0;
}

int stopReactor(void *reactor) {
    Reactor *r = (Reactor *)reactor;
    r->running = 0;
    return 0;
}

void runReactor(void *reactor) {
    Reactor *r = (Reactor *)reactor;

    while (r->running) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        int maxfd = 0;

        for (int i = 0; i < MAX_FDS; i++) {
            if (r->handlers[i].active) {
                FD_SET(i, &read_fds);
                if (i > maxfd) maxfd = i;
            }
        }

        struct timeval timeout = {1, 0}; // 1 second timeout
        int ready = select(maxfd + 1, &read_fds, NULL, NULL, &timeout);

        if (ready < 0) {
            perror("select");
            break;
        }

        for (int i = 0; i <= maxfd; i++) {
            if (r->handlers[i].active && FD_ISSET(i, &read_fds)) {
                r->handlers[i].func(i);
            }
        }
    }
}