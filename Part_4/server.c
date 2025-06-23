#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/select.h>

#define PORT 9034
#define BUF_SIZE 1024

typedef struct {
    float x, y;
} Point;

Point* points = NULL;
int point_count = 0;

int compare(const void *a, const void *b) {
    Point *p1 = (Point *)a, *p2 = (Point *)b;
    if (p1->x != p2->x) return (p1->x < p2->x) ? -1 : 1;
    return (p1->y < p2->y) ? -1 : 1;
}

float cross(Point o, Point a, Point b) {
    return (a.x - o.x)*(b.y - o.y) - (a.y - o.y)*(b.x - o.x);
}

float polygon_area(Point *poly, int n) {
    float area = 0.0;
    for (int i = 0; i < n; i++) {
        Point p1 = poly[i], p2 = poly[(i + 1) % n];
        area += (p1.x * p2.y) - (p1.y * p2.x);
    }
    return fabs(area) / 2.0;
}

void compute_convex_hull(int client_fd) {
    char result[64];
    if (point_count < 3) {
        snprintf(result, sizeof(result), "Invalid: at least 3 points required.\n");
        send(client_fd, result, strlen(result), 0);
        return;
    }

    qsort(points, point_count, sizeof(Point), compare);
    Point* hull = malloc(2 * point_count * sizeof(Point));
    int k = 0;

    for (int i = 0; i < point_count; i++) {
        while (k >= 2 && cross(hull[k - 2], hull[k - 1], points[i]) <= 0) k--;
        hull[k++] = points[i];
    }

    int t = k + 1;
    for (int i = point_count - 2; i >= 0; i--) {
        while (k >= t && cross(hull[k - 2], hull[k - 1], points[i]) <= 0) k--;
        hull[k++] = points[i];
    }

    float area = polygon_area(hull, k - 1);
    snprintf(result, sizeof(result), "Convex hull area: %.1f\n", area);
    send(client_fd, result, strlen(result), 0);
    free(hull);
}

void handle_command(int client_fd, char* cmd) {
    if (strncmp(cmd, "Newgraph", 8) == 0) {
        int n;
        sscanf(cmd, "Newgraph %d", &n);
        free(points);
        points = malloc(n * sizeof(Point));
        point_count = 0;

        char buf[BUF_SIZE];
        for (int i = 0; i < n; i++) {
            send(client_fd, "Enter point x,y:\n", 17, 0);
            int len = recv(client_fd, buf, BUF_SIZE - 1, 0);
            buf[len] = '\0';
            float x, y;
            if (sscanf(buf, "%f,%f", &x, &y) == 2) {
                points[point_count++] = (Point){x, y};
            } else {
                send(client_fd, "Invalid format. Try again.\n", 28, 0);
                i--;
            }
        }

        char confirm[64];
        snprintf(confirm, sizeof(confirm), "Graph initialized with %d points.\n", point_count);
        send(client_fd, confirm, strlen(confirm), 0);

    } else if (strncmp(cmd, "Newpoint", 8) == 0) {
        float x, y;
        if (sscanf(cmd, "Newpoint %f,%f", &x, &y) == 2) {
            points = realloc(points, (point_count + 1) * sizeof(Point));
            points[point_count++] = (Point){x, y};
            send(client_fd, "Point added.\n", 13, 0);
        }

    } else if (strncmp(cmd, "Removepoint", 11) == 0) {
        float x, y;
        if (sscanf(cmd, "Removepoint %f,%f", &x, &y) == 2) {
            int found = -1;
            for (int i = 0; i < point_count; i++) {
                if (points[i].x == x && points[i].y == y) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                for (int i = found; i < point_count - 1; i++) {
                    points[i] = points[i + 1];
                }
                point_count--;
                points = realloc(points, point_count * sizeof(Point));
                send(client_fd, "Point removed.\n", 15, 0);
            } else {
                send(client_fd, "Point not found.\n", 17, 0);
            }
        }

    } else if (strncmp(cmd, "CH", 2) == 0) {
        compute_convex_hull(client_fd);
    } else {
        send(client_fd, "Unknown command.\n", 17, 0);
    }
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(listener, (struct sockaddr*)&addr, sizeof(addr));
    listen(listener, 10);

    fd_set master, read_fds;
    FD_ZERO(&master);
    FD_SET(listener, &master);
    int fdmax = listener;

    printf("Server running on port %d...\n", PORT);

    while (1) {
        read_fds = master;
        select(fdmax + 1, &read_fds, NULL, NULL, NULL);

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener) {
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    int newfd = accept(listener, (struct sockaddr*)&client_addr, &addrlen);
                    FD_SET(newfd, &master);
                    if (newfd > fdmax) fdmax = newfd;
                    char *welcome = "Welcome to the convex hull server!\nCommands: Newgraph N, CH, Newpoint x,y, Removepoint x,y\n";
                    send(newfd, welcome, strlen(welcome), 0);
                } else {
                    char buf[BUF_SIZE];
                    int nbytes = recv(i, buf, BUF_SIZE - 1, 0);
                    if (nbytes <= 0) {
                        close(i);
                        FD_CLR(i, &master);
                    } else {
                        buf[nbytes] = '\0';
                        handle_command(i, buf);
                    }
                }
            }
        }
    }

    return 0;
}