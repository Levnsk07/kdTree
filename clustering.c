#include <stdio.h>
#include <stdlib.h>
#include "kdTree.c"

typedef struct {
    Tree **points;
    int count;
    int capacity;
} Neighbors;

Neighbors* createNeighbors(int capacity) {
    Neighbors* neighbors = (Neighbors*)malloc(sizeof(Neighbors));
    neighbors->points = (Tree**)malloc(capacity * sizeof(Tree*));
    neighbors->count = 0;
    neighbors->capacity = capacity;
    return neighbors;
}

void freeNeighbors(Neighbors* neighbors) {
    free(neighbors->points);
    free(neighbors);
}

void addNeighbor(Neighbors* neighbors, Tree* point) {
    if (neighbors->count < neighbors->capacity) {
        neighbors->points[neighbors->count++] = point;
    }
}

void findNeighbors(Tree* tree, double* point, int dim, double eps, Neighbors* neighbors) {
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