#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    float x, y;
} Point;

Point* points = NULL;
int point_count = 0;

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

void compute_convex_hull() {
    if (point_count < 3) {
        printf("0.0\n");
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
    printf("%.1f\n", area);

    free(hull);
}

void add_point(float x, float y) {
    points = realloc(points, (point_count + 1) * sizeof(Point));
    points[point_count].x = x;
    points[point_count].y = y;
    point_count++;
}

void remove_point(float x, float y) {
    int index = -1;
    for (int i = 0; i < point_count; i++) {
        if (points[i].x == x && points[i].y == y) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        for (int i = index; i < point_count - 1; i++) {
            points[i] = points[i + 1];
        }
        point_count--;
        points = realloc(points, point_count * sizeof(Point));
    }
}

int main() {
    char command[64];
    while (fgets(command, sizeof(command), stdin)) {
        if (strncmp(command, "Newgraph", 8) == 0) {
            int n;
            sscanf(command, "Newgraph %d", &n);
            free(points);
            points = malloc(n * sizeof(Point));
            point_count = 0;
            for (int i = 0; i < n; i++) {
                float x, y;
                if (scanf("%f,%f\n", &x, &y) == 2) {
                    points[point_count++] = (Point){x, y};
                }
            }
        } else if (strncmp(command, "CH", 2) == 0) {
            compute_convex_hull();
        } else if (strncmp(command, "Newpoint", 8) == 0) {
            float x, y;
            if (sscanf(command, "Newpoint %f,%f", &x, &y) == 2) {
                add_point(x, y);
            }
        } else if (strncmp(command, "Removepoint", 11) == 0) {
            float x, y;
            if (sscanf(command, "Removepoint %f,%f", &x, &y) == 2) {
                remove_point(x, y);
            }
        }
    }

    free(points);
    return 0;
}