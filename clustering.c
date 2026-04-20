#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "kdTree.h"


typedef struct {
    int clusterId; // ID кластера (-1: шум, 0: не присвоен)
    int isVisited; // Посещена ли точка
    int isCore; // Является ли точка ядром (>= minPts)
    int isNoise; // Шумовая точка
} PointInfo;



void findNeighborsInRadius(Tree *node, double *point, int dim, double eps, Tree ***neighbors, int *numNeighbors) { // если точка в досигаемости, то добавляет в соседи
    if (node == NULL) return;

    double dist = distance(node->point, point, dim);
    if (dist <= eps) {
        (*neighbors)[*numNeighbors] = node;
        (*numNeighbors)++;
        Tree **temp = realloc(*neighbors, (*numNeighbors + 1) * sizeof(Tree *));
        if (temp) *neighbors = temp;
    }

    int axis = node->splitByDim;
    double splitDist = fabs(point[axis] - node->point[axis]);

    if (point[axis] < node->point[axis]) {
        findNeighborsInRadius(node->left, point, dim, eps, neighbors, numNeighbors);
        if (splitDist <= eps) {
            findNeighborsInRadius(node->right, point, dim, eps, neighbors, numNeighbors);
        }
    } else {
        findNeighborsInRadius(node->right, point, dim, eps, neighbors, numNeighbors);
        if (splitDist <= eps) {
            findNeighborsInRadius(node->left, point, dim, eps, neighbors, numNeighbors);
        }
    }
}

int* dbscan(Tree *root, double **points, int numPoints, int dim, double eps, int minPts) {
    PointInfo *clusterInfo = calloc(numPoints, sizeof(PointInfo));
    int clusterId = 0;

    for (int i = 0; i < numPoints; i++) {
        if (clusterInfo[i].isVisited) continue;
        clusterInfo[i].isVisited = 1;

        Tree **neighbors = malloc(numPoints * sizeof(Tree *));
        int numNeighbors = 0;
        findNeighborsInRadius(root, points[i], dim, eps, &neighbors, &numNeighbors);

        if (numNeighbors < minPts) {
            clusterInfo[i].isNoise = 1;
        } else {
            clusterId++;
            clusterInfo[i].clusterId = clusterId;
            clusterInfo[i].isCore = 1;

            for (int j = 0; j < numNeighbors; j++) {
                int neighborIndex = -1;
                for (int k = 0; k < numPoints; k++) {
                    if (neighbors[j]->point == points[k]) {
                        neighborIndex = k;
                        break;
                    }
                }
                if (neighborIndex != -1 && !clusterInfo[neighborIndex].isVisited) {
                    clusterInfo[neighborIndex].isVisited = 1;
                    if (numNeighbors >= minPts) {
                        clusterInfo[neighborIndex].clusterId = clusterId;
                        clusterInfo[neighborIndex].isCore = 1;
                    }
                }
            }
        }
        free(neighbors);
    }
    int *result = malloc(numPoints * sizeof(int));
    for (int i = 0; i < numPoints; i++) {
        result[i] = clusterInfo[i].isNoise ? -1 : clusterInfo[i].clusterId;
    }
    free(clusterInfo);
    return result;
}