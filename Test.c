#include <stdio.h>
#include <stdlib.h>

#include "kdTree.c"
#include "clustering.c"

int main() {
    int dim = 2;
    int numPoints = 5;
    double **points = malloc(numPoints * sizeof(double *));
    for (int i = 0; i < numPoints; i++) {
        points[i] = malloc(dim * sizeof(double));
    }

    // Пример данных: точки в 2D пространстве
    points[0][0] = 1.0;
    points[0][1] = 1.0;
    points[1][0] = 1.5;
    points[1][1] = 1.8;
    points[2][0] = 5.0;
    points[2][1] = 8.0;
    points[3][0] = 8.0;
    points[3][1] = 8.0;
    points[4][0] = 3.0;
    points[4][1] = 3.0;

    // Построение KD-дерева
    Tree *root = initKDTree(2);
    buildKDTree(&root, points, numPoints, 0, dim, 0);

    // Параметры DBSCAN
    double eps = 2.0;
    int minPts = 2;

    // Запуск DBSCAN
    int *cluster = dbscan(root, points, numPoints, dim, eps, minPts);

    for (int i = 0; i < 4; i++) {
        printf("cluster[%d]\n",cluster[i]);
    }

    // Освобождение памяти
    for (int i = 0; i < numPoints; i++) {
        free(points[i]);
    }
    free(points);

    return 0;
}
