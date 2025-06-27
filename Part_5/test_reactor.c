#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "patterns/reactor.h"

void handle_stdin(int fd) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        printf("You typed: %s", buffer);
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Exiting reactor.\n");
            stopReactor(NULL);
        }
    }
}

int main() {
    void *reactor = startReactor();
    if (!reactor) {
        fprintf(stderr, "Failed to start reactor\n");
        return 1;
    }

    addFdToReactor(reactor, STDIN_FILENO, handle_stdin);
    printf("Type anything (or 'exit' to quit):\n");
    runReactor(reactor);

    free(reactor);
    return 0;
}