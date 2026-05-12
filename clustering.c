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


static double distance2(double *point1, double *point2, int dim) { // TODO need to make new func
    double dist = 0.0;
    for (int i = 0; i < dim; i++) {
        double diff = point1[i] - point2[i];
        dist += diff * diff;
    }
    return dist;
}

void findNeighborsInRadius(Tree *node, double *point, int dim, double eps, Tree ***neighbors, int *numNeighbors) { // если точка в досигаемости, то добавляет в соседи
    if (node == NULL) return;

    double dist = distance2(node->point, point, dim);
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

// ============== C-MEANS ======================

static double **createMatrix(int rows, int cols){
    double **matrix = (double**)malloc(rows*sizeof(double*));
    for(int i=0; i < rows; i++)
        matrix[i] = (double*)malloc(cols*sizeof(double));
    return matrix;
}

static void freeMatrix(double **matrix, int rows){
    if(matrix == NULL)
        return;

    for(int i=0; i < rows; i++)
        free(matrix[i]);
    free(matrix);
}

static void copyMatrix(double **matrixSrc, double **matrixDest, int rows, int cols){
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++)
            matrixDest[i][j] = matrixSrc[i][j];
    }
}

static double euclideanDistance(double *point1, double *point2, int dim) {
    double dist = 0.0, diff = 0.0;
    for (int i = 0; i < dim; i++) {
        diff = point1[i] - point2[i];
        dist += diff * diff;
    }
    return sqrt(dist);
}

static void initMembership(Tree *node, int clustersCount){
    if(node == NULL || clustersCount <= 0)
        return;

    if(node->memberShip == NULL)
        node->memberShip = (double*)malloc(clustersCount*sizeof(double));
    double sum=0.0;
    for(int i=0; i < clustersCount; i++){
        node->memberShip[i] = rand() + 1.0;
        sum += node->memberShip[i];
    }
    for(int i=0; i < clustersCount; i++)
        node->memberShip[i] /= sum;

    initMembership(node->left, clustersCount);
    initMembership(node->right, clustersCount);
}

static void clearCenters(double **centers, double *denominators, int clustersCount, int dim){
    for(int i=0; i < clustersCount; i++){
        denominators[i] = 0.0;
        for(int j=0; j < dim; j++)
            centers[i][j] = 0.0;
    }
}

static void accumulateCenters(Tree *node, double **centers, double *denominators, int clustersCount, int dim, double m){
    if(node == NULL)
        return;

    double weight = 0.0;
    for(int i=0; i < clustersCount; i++){
        weight = pow(node->memberShip[i], m);
        denominators[i] += weight;
        for(int j=0; j < dim; j++)
            centers[i][j] += weight*node->point[j];
    }
    accumulateCenters(node->left, centers, denominators, clustersCount, dim, m);
    accumulateCenters(node->right, centers, denominators, clustersCount, dim, m);
}

static void normalizeCenters(double **centers, double *denominators, int clustersCount, int dim){
    for(int i=0; i < clustersCount; i++){
        for(int j=0; j < dim; j++){
            if(denominators[i] != 0.0)
                centers[i][j] /= denominators[i];
        }
    }
}

static void updateCenters(Tree *node, double **centers, double *denominators, int clustersCount, int dim, double m){
    clearCenters(centers, denominators, clustersCount, dim);
    accumulateCenters(node, centers, denominators, clustersCount, dim, m);
    normalizeCenters(centers, denominators, clustersCount, dim);
}

static void updateMembership(Tree *node, double **centers, int clustersCount, int dim, double m){
    if(node == NULL)
        return;
    double *distances = (double*)malloc(clustersCount*sizeof(double));

    int zeroDistanceCluster = -1;

    for(int i=0; i < clustersCount; i++){
        distances[i] = euclideanDistance(node->point, centers[i], dim);

        if(distances[i] == 0.0){
            zeroDistanceCluster = i;
            break;
        }
    }

    if(zeroDistanceCluster != -1){
        for(int i=0; i < clustersCount; i++){
            if(i == zeroDistanceCluster)
                node->memberShip[i] = 1.0;
            else
                node->memberShip[i] = 0.0;
        }
    }
    else{
        double exponent = 2.0 / (m-1.0);
        double sum = 0.0;
        for(int i=0; i < clustersCount; i++){
            sum = 0.0;
            for(int j=0; j < clustersCount; j++)
                sum += pow(distances[i] / distances[j], exponent);
            node->memberShip[i] = 1.0 / sum;
        }
    }

    free(distances);

    updateMembership(node->left, centers, clustersCount, dim, m);
    updateMembership(node->right, centers, clustersCount, dim, m);
}

static double maxCenterShift(double **oldCenters, double **centers, int clustersCount, int dim){
    double shift=0.0, maxShift = 0.0;
    for(int i=0; i < clustersCount; i++){
        shift = euclideanDistance(centers[i], oldCenters[i], dim);
        if(shift > maxShift)
            maxShift = shift;
    }
    return maxShift;
}

static void createClusterIdx(Tree *root, int clustersCount){
    if(root == NULL || clustersCount <= 0)
        return;

    int bestIdx = 0;
    double bestMembership = root->memberShip[0];

    for(int i=1; i < clustersCount; i++){
        if(root->memberShip[i] > bestMembership){
            bestMembership = root->memberShip[i];
            bestIdx = i;
        }
    }

    root->clusterIdx = bestIdx;

    createClusterIdx(root->left, clustersCount);
    createClusterIdx(root->right, clustersCount);
}


bool fuzzyCMeans(Tree *root, int pointsCount, int dim, int clustersCount){
    if(root == NULL || pointsCount <= 0 || dim <= 0 || clustersCount <= 0 || clustersCount > pointsCount)
        return false;

    srand(0);

    double m = 2.0;
    double eps = 0.001;
    int maxIters = 1000;

    double **centers = createMatrix(clustersCount, dim);
    double **oldCenters = createMatrix(clustersCount, dim);
    double *denominators = (double*)malloc(clustersCount*sizeof(double));

    initMembership(root, clustersCount);
    updateCenters(root, centers, denominators, clustersCount, dim, m);
    double shift = 0.0;
    for(int i=0; i < maxIters; i++){
        copyMatrix(centers, oldCenters, clustersCount, dim);

        updateMembership(root, centers, clustersCount, dim, m);
        updateCenters(root, centers, denominators, clustersCount, dim, m);

        shift = maxCenterShift(oldCenters, centers, clustersCount, dim);

        if(shift < eps)
            break;
    }

    createClusterIdx(root, clustersCount);

    freeMatrix(centers, clustersCount);
    freeMatrix(oldCenters, clustersCount);
    free(denominators);

    return true;
}