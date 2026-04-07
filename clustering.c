#include <stdio.h>
#include <stdlib.h>
#include "kdTree.c"

typedef struct {
    Tree **points;
    int count;
    int capacity;
} Neighbors;

Neighbors *createNeighbors(int capacity) {
    Neighbors *neighbors = (Neighbors *) malloc(sizeof(Neighbors));
    neighbors->points = (Tree **) malloc(capacity * sizeof(Tree *));
    neighbors->count = 0;
    neighbors->capacity = capacity;
    return neighbors;
}

void freeNeighbors(Neighbors *neighbors) {
    free(neighbors->points);
    free(neighbors);
}

void addNeighbor(Neighbors *neighbors, Tree *point) {
    if (neighbors->count < neighbors->capacity) {
        neighbors->points[neighbors->count++] = point;
    }
}

void findNeighbors(Tree *tree, double *point, int dim, double eps, Neighbors *neighbors) {
    if (tree == NULL) {
        return;
    }

    double dist = distance(tree->point, point, dim);
    if (dist <= eps * eps) {
        addNeighbor(neighbors, tree);
    }

    int axis = tree->splitByDim;
    if (point[axis] - eps <= tree->point[axis]) {
        findNeighbors(tree->left, point, dim, eps, neighbors);
    }
    if (point[axis] + eps >= tree->point[axis]) {
        findNeighbors(tree->right, point, dim, eps, neighbors);
    }
}

void expandCluster(Tree *tree, Tree *point, int dim, double eps, int minPts, int clusterId, int *clusterIds,
                   double **points, int numPoints) {
    Neighbors *neighbors = createNeighbors(numPoints);
    findNeighbors(tree, point->point, dim, eps, neighbors);

    if (neighbors->count < minPts) {
        freeNeighbors(neighbors);
        return;
    }

    for (int i = 0; i < neighbors->count; i++) {
        Tree *neighbor = neighbors->points[i];
        int neighborIndex = -1;
        for (int j = 0; j < numPoints; j++) {
            int isEqual = 1;
            for (int k = 0; k < dim; k++) {
                if (neighbor->point[k] != points[j][k]) {
                    isEqual = 0;
                    break;
                }
            }
            if (isEqual) {
                neighborIndex = j;
                break;
            }
        }

        if (neighborIndex != -1 && clusterIds[neighborIndex] == -1) {
            clusterIds[neighborIndex] = clusterId;
            Tree *neighborNode = getPointInTree(tree, points[neighborIndex], dim);
            expandCluster(tree, neighborNode, dim, eps, minPts, clusterId, clusterIds, points, numPoints);
        }
    }

    freeNeighbors(neighbors);
}

void dbscan(Tree *tree, int dim, double eps, int minPts, int *clusterIds, int *clusterCount, int numPoints,
            double **points) {
    int clusterId = 0;
    for (int i = 0; i < numPoints; i++) {
        if (clusterIds[i] != -1) {
            continue;
        }

        Tree *currentPointNode = getPointInTree(tree, points[i], dim);
        Neighbors *neighbors = createNeighbors(numPoints);
        findNeighbors(tree, points[i], dim, eps, neighbors);

        if (neighbors->count < minPts) {
            clusterIds[i] = -2;
            freeNeighbors(neighbors);
            continue;
        }

        clusterIds[i] = clusterId;
        expandCluster(tree, currentPointNode, dim, eps, minPts, clusterId, clusterIds, points, numPoints);

        clusterId++;
        freeNeighbors(neighbors);
    }
    *clusterCount = clusterId;
}
