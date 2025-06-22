#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//set to 0 to use merge sort
#define USE_QSORT 0

typedef struct {
    float x, y;
} Point;

int compare(const void *a, const void *b) {
    Point *p1 = (Point *)a, *p2 = (Point *)b;
    if (p1->x != p2->x)
        return (p1->x < p2->x) ? -1 : 1;
    return (p1->y < p2->y) ? -1 : 1;
}

int compare_points(Point a, Point b) {
    if (a.x != b.x)
        return (a.x < b.x) ? -1 : 1;
    return (a.y < b.y) ? -1 : 1;
}

void merge(Point *arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    Point *L = malloc(n1 * sizeof(Point));
    Point *R = malloc(n2 * sizeof(Point));
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (compare_points(L[i], R[j]) <= 0)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L);
    free(R);
}

void merge_sort(Point *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
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
    for (int i = 0; i < n;) {
        printf("please enter a point (x,y): ");
        if (scanf("%f,%f", &points[i].x, &points[i].y) == 2) {
            if (points[i].x == -1 || points[i].y == -1) {
                printf("User aborted input. Exiting.\n");
                free(points);
                return 0;
            }
            i++;
        } else {
            printf("invalid format.\n");
            while (getchar() != '\n');
        }
    }

#if USE_QSORT
    qsort(points, n, sizeof(Point), compare);
#else
    merge_sort(points, 0, n - 1);
#endif

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
    printf("result: %.1f\n", area);

    free(points);
    free(hull);
    return 0;
}