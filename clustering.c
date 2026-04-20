#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "kdTree.h"

typedef struct {
    Tree **points;
    int count;
    int capacity;
} Neighbors;

Neighbors *createNeighbors(int capacity) {
    Neighbors *neighbors = (Neighbors *)malloc(sizeof(Neighbors));
    neighbors->points = (Tree **)malloc(capacity * sizeof(Tree *));
    neighbors->count = 0;
    neighbors->capacity = capacity;
    return neighbors;
}


void freeNeighbors(Neighbors *neighbors) {
    if (neighbors) {
        free(neighbors->points);
        free(neighbors);
    }
}


void addNeighbor(Neighbors *neighbors, Tree *point) {
    if (neighbors->count < neighbors->capacity) {
        neighbors->points[neighbors->count++] = point;
    }
}


void findNeighbors(Tree *tree, double *point, int dim, double eps, Neighbors *neighbors) {
    if (tree == NULL) return;

    double dist = distance(tree->point, point, dim);
    if (dist <= eps) {
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


void expandCluster(Tree *tree, Tree *point, int dim, double eps, int minPts, int clusterId,
                   int *clusterIds, double **points, int numPoints) {
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
            expandCluster(tree, neighbor, dim, eps, minPts, clusterId, clusterIds, points, numPoints);
        }
    }
    freeNeighbors(neighbors);
}


int *dbscan(Tree *tree, double **points, int num_points, int dim, double eps, int minPts) {
    int *clusterIds = (int *)calloc(num_points, sizeof(int));
    int clusterId = 0;

    for (int i = 0; i < num_points; i++) {
        if (clusterIds[i] != -1) continue;

        Neighbors *neighbors = createNeighbors(num_points);
        findNeighbors(tree, points[i], dim, eps, neighbors);

        if (neighbors->count < minPts) {
            clusterIds[i] = -1; // Шум
        } else {
            clusterId++;
            clusterIds[i] = clusterId;
            expandCluster(tree, neighbors->points[0], dim, eps, minPts, clusterId, clusterIds, points, num_points);
        }
        freeNeighbors(neighbors);
    }

    return clusterIds;
}


FuzzyCMeansResult cmean(double **points, int num_points, int dim, int num_clusters, double m) {
    FuzzyCMeansResult result;
    result.centroids = (double **)malloc(num_clusters * sizeof(double *));
    result.memberships = (double **)malloc(num_points * sizeof(double *));
    result.num_clusters = num_clusters;
    result.num_points = num_points;

    for (int i = 0; i < num_clusters; i++) {
        result.centroids[i] = (double *)malloc(dim * sizeof(double));
        for (int j = 0; j < dim; j++) {
            result.centroids[i][j] = (double)rand() / RAND_MAX * 10.0;
        }
    }

    for (int i = 0; i < num_points; i++) {
        result.memberships[i] = (double *)malloc(num_clusters * sizeof(double));
    }

    for (int iter = 0; iter < 100; iter++) {
        for (int i = 0; i < num_points; i++) {
            for (int j = 0; j < num_clusters; j++) {
                double sum = 0.0;
                for (int k = 0; k < num_clusters; k++) {
                    double dist_ij = distance(points[i], result.centroids[j], dim);
                    double dist_ik = distance(points[i], result.centroids[k], dim);
                    sum += pow(dist_ij / dist_ik, 2.0 / (m - 1.0));
                }
                result.memberships[i][j] = 1.0 / sum;
            }
        }

        for (int j = 0; j < num_clusters; j++) {
            double sum_weights = 0.0;
            for (int k = 0; k < dim; k++) {
                result.centroids[j][k] = 0.0;
            }
            for (int i = 0; i < num_points; i++) {
                double weight = pow(result.memberships[i][j], m);
                for (int k = 0; k < dim; k++) {
                    result.centroids[j][k] += weight * points[i][k];
                }
                sum_weights += weight;
            }
            for (int k = 0; k < dim; k++) {
                result.centroids[j][k] /= sum_weights;
            }
        }
    }

    return result;
}


void freeFuzzyCMeansResult(FuzzyCMeansResult *result) {
    if (!result) return;
    for (int i = 0; i < result->num_clusters; i++) {
        free(result->centroids[i]);
    }
    free(result->centroids);
    for (int i = 0; i < result->num_points; i++) {
        free(result->memberships[i]);
    }
    free(result->memberships);
}