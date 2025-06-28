#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include "patterns/reactor.h"
#include "patterns/proactor.h"

#define DEFAULT_PORT 9034
#define BUFFER_SIZE 1024

typedef struct {
    float x, y;
} Point;

Point *points = NULL;
int point_count = 0;

pthread_mutex_t graph_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t area_cond = PTHREAD_COND_INITIALIZER;

float last_area = 0.0;
int area_over_100 = 0;

int compare(const void *a, const void *b) {
    Point *p1 = (Point *)a, *p2 = (Point *)b;
    if (p1->x != p2->x) return (p1->x < p2->x) ? -1 : 1;
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

float compute_convex_hull(int client_fd) {
    if (point_count < 3) {
        send(client_fd, "Not enough points\n", 18, 0);
        return 0.0;
    }

    Point *hull = malloc(2 * point_count * sizeof(Point));
    int k = 0;

    qsort(points, point_count, sizeof(Point), compare);

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
    char msg[64];
    snprintf(msg, sizeof(msg), "Convex Hull Area: %.1f\n", area);
    send(client_fd, msg, strlen(msg), 0);
    free(hull);

    return area;
}

int thread_counter = 1;

void *area_monitor_thread(void *arg) {
    int tid = __sync_fetch_and_add(&thread_counter, 1);
    printf("Thread %d: Area monitor started\n", tid);

    while (1) {
        pthread_mutex_lock(&graph_mutex);
        pthread_cond_wait(&area_cond, &graph_mutex);

        if (last_area >= 100.0 && !area_over_100) {
            printf("At least 100 units belongs to CH\n");
            area_over_100 = 1;
        } else if (last_area < 100.0 && area_over_100) {
            printf("At least 100 units no longer belongs to CH\n");
            area_over_100 = 0;
        }

        pthread_mutex_unlock(&graph_mutex);
    }
    return NULL;
}

void *client_thread(int *client_fd_ptr) {
    int tid = __sync_fetch_and_add(&thread_counter, 1);
    printf("Thread %d: handling a client\n", tid);

    int client_fd = *client_fd_ptr;
    free(client_fd_ptr);

    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) break;

        pthread_mutex_lock(&graph_mutex);

        if (strncmp(buffer, "Newgraph", 8) == 0) {
            int n;
            sscanf(buffer, "Newgraph %d", &n);
            free(points);
            points = malloc(n * sizeof(Point));
            point_count = 0;
            send(client_fd, "Send points as x,y (one per line):\n", 36, 0);
            for (int i = 0; i < n; i++) {
                memset(buffer, 0, BUFFER_SIZE);
                recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
                float x, y;
                if (sscanf(buffer, "%f,%f", &x, &y) == 2) {
                    points[point_count++] = (Point){x, y};
                }
            }
            send(client_fd, "Graph created successfully\n", 27, 0);

        } else if (strncmp(buffer, "Newpoint", 8) == 0) {
            float x, y;
            if (sscanf(buffer, "Newpoint %f,%f", &x, &y) == 2) {
                points = realloc(points, (point_count + 1) * sizeof(Point));
                points[point_count++] = (Point){x, y};
                send(client_fd, "Point added\n", 12, 0);
            }

        } else if (strncmp(buffer, "Removepoint", 11) == 0) {
            float x, y;
            if (sscanf(buffer, "Removepoint %f,%f", &x, &y) == 2) {
                for (int i = 0; i < point_count; i++) {
                    if (points[i].x == x && points[i].y == y) {
                        for (int j = i; j < point_count - 1; j++) {
                            points[j] = points[j + 1];
                        }
                        point_count--;
                        points = realloc(points, point_count * sizeof(Point));
                        break;
                    }
                }
                send(client_fd, "Point removed\n", 14, 0);
            }

        } else if (strncmp(buffer, "CH", 2) == 0) {
            last_area = compute_convex_hull(client_fd);
            pthread_cond_signal(&area_cond);
        } else {
            send(client_fd, "Unknown command\n", 16, 0);
        }

        pthread_mutex_unlock(&graph_mutex);
    }

    close(client_fd);
    return NULL;
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    printf("Using port: %d\n", port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };
    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(sockfd, 10);
  
    printf("Server running with proactor on port %d...\n", port);

    pthread_t accept_tid = startProactor(sockfd, client_thread);

    pthread_t monitor_tid;
    pthread_create(&monitor_tid, NULL, area_monitor_thread, NULL);


    pthread_join(accept_tid, NULL);
    pthread_join(monitor_tid, NULL);

    free(points);
    return 0;
}