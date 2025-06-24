#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "reactor.h"
#include <math.h>

typedef struct {
    float x, y;
} Point;

Point* points = NULL;
int point_count = 0;

void add_point(float x, float y) {
    points = realloc(points, (point_count + 1) * sizeof(Point));
    points[point_count].x = x;
    points[point_count].y = y;
    point_count++;
}

void remove_point(float x, float y) {
    int i;
    for (i = 0; i < point_count; i++) {
        if (points[i].x == x && points[i].y == y) {
            for (int j = i; j < point_count - 1; j++) {
                points[j] = points[j + 1];
            }
            point_count--;
            points = realloc(points, point_count * sizeof(Point));
            break;
        }
    }
}

int compare(const void *a, const void *b) {
    Point *p1 = (Point *)a, *p2 = (Point *)b;
    if (p1->x != p2->x)
        return (p1->x < p2->x) ? -1 : 1;
    return (p1->y < p2->y) ? -1 : 1;
}

float cross(Point o, Point a, Point b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

float polygon_area(Point *poly, int n) {
    float area = 0.0;
    for (int i = 0; i < n; i++) {
        Point p1 = poly[i];
        Point p2 = poly[(i + 1) % n];
        area += (p1.x * p2.y) - (p1.y * p2.x);
    }
    return fabs(area) / 2.0;
}

void compute_convex_hull(int fd) {
    if (point_count < 3) {
        send(fd, "invalid amount of points, at least 3\n", 37, 0);
        return;
    }

    Point* sorted = malloc(point_count * sizeof(Point));
    memcpy(sorted, points, point_count * sizeof(Point));
    qsort(sorted, point_count, sizeof(Point), compare);

    Point* hull = malloc(2 * point_count * sizeof(Point));
    int k = 0;

    for (int i = 0; i < point_count; i++) {
        while (k >= 2 && cross(hull[k - 2], hull[k - 1], sorted[i]) <= 0) k--;
        hull[k++] = sorted[i];
    }

    int t = k + 1;
    for (int i = point_count - 2; i >= 0; i--) {
        while (k >= t && cross(hull[k - 2], hull[k - 1], sorted[i]) <= 0) k--;
        hull[k++] = sorted[i];
    }

    float area = polygon_area(hull, k - 1);
    char response[64];
    snprintf(response, sizeof(response), "Convex hull area: %.1f\n", area);
    send(fd, response, strlen(response), 0);

    free(hull);
    free(sorted);
}

void handle_client(int fd) {
    char buf[1024];
    int nbytes = recv(fd, buf, sizeof(buf) - 1, 0);
    if (nbytes <= 0) {
        close(fd);
        removeFdFromReactor(getReactorInstance(), fd);
        return;
    }

    buf[nbytes] = '\0';

    if (strncmp(buf, "Newgraph", 8) == 0) {
        int n;
        sscanf(buf, "Newgraph %d", &n);
        free(points);
        points = malloc(n * sizeof(Point));
        point_count = 0;

        send(fd, "OK, send points (x,y)\n", 23, 0);
        for (int i = 0; i < n; i++) {
            char line[64];
            int r = recv(fd, line, sizeof(line), 0);
            if (r > 0) {
                float x, y;
                if (sscanf(line, "%f,%f", &x, &y) == 2) {
                    points[point_count++] = (Point){x, y};
                }
            }
        }
        send(fd, "Graph created.\n", 15, 0);

    } else if (strncmp(buf, "Newpoint", 8) == 0) {
        float x, y;
        if (sscanf(buf, "Newpoint %f,%f", &x, &y) == 2) {
            add_point(x, y);
            send(fd, "Point added.\n", 13, 0);
        }

    } else if (strncmp(buf, "Removepoint", 11) == 0) {
        float x, y;
        if (sscanf(buf, "Removepoint %f,%f", &x, &y) == 2) {
            remove_point(x, y);
            send(fd, "Point removed.\n", 15, 0);
        }

    } else if (strncmp(buf, "CH", 2) == 0) {
        compute_convex_hull(fd);
    } else {
        send(fd, "Unknown command.\n", 17, 0);
    }
}

void accept_connection(int listener_fd) {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int newfd = accept(listener_fd, (struct sockaddr*)&client, &len);
    if (newfd >= 0) {
        send(newfd, "Connected to convex hull server.\n", 33, 0);
        addFdToReactor(getReactorInstance(), newfd, handle_client);
    }
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(9034)
    };

    bind(listener, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(listener, 10);

    void *reactor = startReactor();
    addFdToReactor(reactor, listener, accept_connection);

    runReactor(reactor);
    stopReactor(reactor);
    free(points);
    return 0;
}