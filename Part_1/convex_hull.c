#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    float x, y;
} Point;

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

int main() {
    int n;
    printf("please enter the amount of points (atleast 3):\n");
    scanf("%d", &n);
    getchar();

    if (n < 3) {
     printf("invalid amount of points (atleast 3)\n");
     return 0;
    }

    Point *points = malloc(n * sizeof(Point));
    for (int i = 0; i < n; ) {
     printf("please enter a point (x,y): ");
     if (scanf("%f,%f", &points[i].x, &points[i].y) == 2) {
         i++;
     } else {
            printf("invalid format.\n");
         while (getchar() != '\n');
     }
    }

    qsort(points, n, sizeof(Point), compare);

    Point *hull = malloc(2 * n * sizeof(Point));
    int k = 0;

    for (int i = 0; i < n; i++) {
        while (k >= 2 && cross(hull[k - 2], hull[k - 1], points[i]) <= 0) k--;
        hull[k++] = points[i];
    }

    int t = k + 1;
    for (int i = n - 2; i >= 0; i--) {
        while (k >= t && cross(hull[k - 2], hull[k - 1], points[i]) <= 0) k--;
        hull[k++] = points[i];
    }

    float area = polygon_area(hull, k - 1);
    printf("result: ");
    printf("%.1f\n", area);

    free(points);
    free(hull);
    return 0;
}