#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "kdTree.h"

// ============== C-MEANS ======================

static double **createMatrix(int rows, int cols) {
    double **matrix = (double **) malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++)
        matrix[i] = (double *) malloc(cols * sizeof(double));
    return matrix;
}

static void freeMatrix(double **matrix, int rows) {
    if (matrix == NULL)
        return;

    for (int i = 0; i < rows; i++)
        free(matrix[i]);
    free(matrix);
}

static void copyMatrix(double **matrixSrc, double **matrixDest, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
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

static void initMembership(Tree *node, int clustersCount) {
    if (node == NULL || clustersCount <= 0)
        return;

    if (node->memberShip == NULL)
        node->memberShip = (double *) malloc(clustersCount * sizeof(double));
    double sum = 0.0;
    for (int i = 0; i < clustersCount; i++) {
        node->memberShip[i] = rand() + 1.0;
        sum += node->memberShip[i];
    }
    for (int i = 0; i < clustersCount; i++)
        node->memberShip[i] /= sum;

    initMembership(node->left, clustersCount);
    initMembership(node->right, clustersCount);
}

static void clearCenters(double **centers, double *denominators, int clustersCount, int dim) {
    for (int i = 0; i < clustersCount; i++) {
        denominators[i] = 0.0;
        for (int j = 0; j < dim; j++)
            centers[i][j] = 0.0;
    }
}

static void accumulateCenters(Tree *node, double **centers, double *denominators, int clustersCount, int dim,
                              double m) {
    if (node == NULL)
        return;

    double weight = 0.0;
    for (int i = 0; i < clustersCount; i++) {
        weight = pow(node->memberShip[i], m);
        denominators[i] += weight;
        for (int j = 0; j < dim; j++)
            centers[i][j] += weight * node->point[j];
    }
    accumulateCenters(node->left, centers, denominators, clustersCount, dim, m);
    accumulateCenters(node->right, centers, denominators, clustersCount, dim, m);
}

static void normalizeCenters(double **centers, double *denominators, int clustersCount, int dim) {
    for (int i = 0; i < clustersCount; i++) {
        for (int j = 0; j < dim; j++) {
            if (denominators[i] != 0.0)
                centers[i][j] /= denominators[i];
        }
    }
}

static void updateCenters(Tree *node, double **centers, double *denominators, int clustersCount, int dim, double m) {
    clearCenters(centers, denominators, clustersCount, dim);
    accumulateCenters(node, centers, denominators, clustersCount, dim, m);
    normalizeCenters(centers, denominators, clustersCount, dim);
}

static void updateMembership(Tree *node, double **centers, int clustersCount, int dim, double m) {
    if (node == NULL)
        return;
    double *distances = (double *) malloc(clustersCount * sizeof(double));

    int zeroDistanceCluster = -1;

    for (int i = 0; i < clustersCount; i++) {
        distances[i] = euclideanDistance(node->point, centers[i], dim);

        if (distances[i] == 0.0) {
            zeroDistanceCluster = i;
            break;
        }
    }

    if (zeroDistanceCluster != -1) {
        for (int i = 0; i < clustersCount; i++) {
            if (i == zeroDistanceCluster)
                node->memberShip[i] = 1.0;
            else
                node->memberShip[i] = 0.0;
        }
    } else {
        double exponent = 2.0 / (m - 1.0);
        double sum = 0.0;
        for (int i = 0; i < clustersCount; i++) {
            sum = 0.0;
            for (int j = 0; j < clustersCount; j++)
                sum += pow(distances[i] / distances[j], exponent);
            node->memberShip[i] = 1.0 / sum;
        }
    }

    free(distances);

    updateMembership(node->left, centers, clustersCount, dim, m);
    updateMembership(node->right, centers, clustersCount, dim, m);
}

static double maxCenterShift(double **oldCenters, double **centers, int clustersCount, int dim) {
    double shift = 0.0, maxShift = 0.0;
    for (int i = 0; i < clustersCount; i++) {
        shift = euclideanDistance(centers[i], oldCenters[i], dim);
        if (shift > maxShift)
            maxShift = shift;
    }
    return maxShift;
}

static void createClusterIdx(Tree *root, int clustersCount) {
    if (root == NULL || clustersCount <= 0)
        return;

    int bestIdx = 0;
    double bestMembership = root->memberShip[0];

    for (int i = 1; i < clustersCount; i++) {
        if (root->memberShip[i] > bestMembership) {
            bestMembership = root->memberShip[i];
            bestIdx = i;
        }
    }

    root->clusterIdx = bestIdx;

    createClusterIdx(root->left, clustersCount);
    createClusterIdx(root->right, clustersCount);
}


bool fuzzyCMeans(Tree *root, int pointsCount, int dim, int clustersCount) {
    if (root == NULL || pointsCount <= 0 || dim <= 0 || clustersCount <= 0 || clustersCount > pointsCount)
        return false;

    srand(0);

    double m = 2.0;
    double eps = 0.001;
    int maxIters = 1000;

    double **centers = createMatrix(clustersCount, dim);
    double **oldCenters = createMatrix(clustersCount, dim);
    double *denominators = (double *) malloc(clustersCount * sizeof(double));

    initMembership(root, clustersCount);
    updateCenters(root, centers, denominators, clustersCount, dim, m);
    double shift = 0.0;
    for (int i = 0; i < maxIters; i++) {
        copyMatrix(centers, oldCenters, clustersCount, dim);

        updateMembership(root, centers, clustersCount, dim, m);
        updateCenters(root, centers, denominators, clustersCount, dim, m);

        shift = maxCenterShift(oldCenters, centers, clustersCount, dim);

        if (shift < eps)
            break;
    }

    createClusterIdx(root, clustersCount);

    freeMatrix(centers, clustersCount);
    freeMatrix(oldCenters, clustersCount);
    free(denominators);

    return true;
}

// ============== dbScan ======================

// int clusters = dbscanCluster(root, eps, minPts);


static void resetClusterIdx(Tree *node, int value) {
    if (node == NULL)
        return;

    node->clusterIdx = value;

    resetClusterIdx(node->left, value);
    resetClusterIdx(node->right, value);
}

static void collectUnclassified(Tree *node, Tree ***nodes, int *count) {
    if (node == NULL)
        return;

    Tree **tmp = (Tree **) realloc(*nodes, (*count + 1) * sizeof(Tree *));
    if (tmp == NULL) return;

    *nodes = tmp;
    (*nodes)[*count] = node;
    (*count)++;

    collectUnclassified(node->left, nodes, count);
    collectUnclassified(node->right, nodes, count);
}

static void findNeighborsInRadius(Tree *root, double *point, int dim, double epsSquared, Tree ***neighbors,
                                  int *numNeighbors) {
    if (root == NULL)
        return;

    /* Вычисляем квадрат евклидова расстояния */
    double distSquared = 0.0;

    for (int i = 0; i < dim; i++) {
        double diff = root->point[i] - point[i];
        distSquared += diff * diff;
    }

    /* Если точка находится внутри радиуса */
    if (distSquared <= epsSquared) {
        Tree **tmp =
                (Tree **) realloc(*neighbors,
                                  (*numNeighbors + 1) *
                                  sizeof(Tree *));
        if (tmp != NULL) {
            *neighbors = tmp;
            (*neighbors)[*numNeighbors] = root;
            (*numNeighbors)++;
        }
    }

    /* Ось разбиения текущего узла */
    int axis = root->splitByDim;

    /* Расстояние до разделяющей гиперплоскости */
    double delta = point[axis] - root->point[axis];
    double deltaSquared = delta * delta;

    /* Обход ближней ветви */
    if (delta < 0.0)
        findNeighborsInRadius(root->left, point, dim, epsSquared, neighbors, numNeighbors);
    else
        findNeighborsInRadius(root->right, point, dim, epsSquared, neighbors, numNeighbors);

    /* Если сфера пересекает плоскость — ищем и во второй ветви */
    if (deltaSquared <= epsSquared) {
        if (delta < 0.0)
            findNeighborsInRadius(root->right, point, dim, epsSquared, neighbors, numNeighbors);
        else
            findNeighborsInRadius(root->left, point, dim, epsSquared, neighbors, numNeighbors);
    }
}
 // MAIN
int dbscanCluster(Tree *root, double eps, int minPts, int count) {
    if (root == NULL || count <= 0 || root->dim <= 0 || eps < 0.0 || minPts <= 0) return 0;

    const int UNCLASSIFIED = -2;
    const int NOISE = -1;

    double epsSquared = eps * eps;
    int currentCluster = 0;

    /* 1. Сброс clusterIdx для всех узлов */
    resetClusterIdx(root, UNCLASSIFIED);

    /* 2. Собираем все узлы дерева */
    Tree **allNodes = NULL;
    int nodeCount = 0;
    collectUnclassified(root, &allNodes, &nodeCount);

    if (allNodes == NULL || nodeCount == 0) {
        free(allNodes);
        return 0;
    }

    /* 3. Основной алгоритм DBSCAN */
    for (int i = 0; i < nodeCount; i++) {
        Tree *start = allNodes[i];

        /* Уже обработана */
        if (start->clusterIdx != UNCLASSIFIED)
            continue;

        /* Находим соседей */
        Tree **neighbors = (Tree **) malloc(sizeof(Tree *));
        if (neighbors == NULL)
            continue;

        int numNeighbors = 0;

        findNeighborsInRadius(root, start->point, root->dim, epsSquared, &neighbors, &numNeighbors);


        if (numNeighbors < minPts) {
            start->clusterIdx = NOISE;
            free(neighbors);
            continue;
        }


        currentCluster++;
        start->clusterIdx = currentCluster;


        for (int j = 0; j < numNeighbors; j++) {
            neighbors[j]->clusterIdx = currentCluster;
        }


        int queueIndex = 0;

        while (queueIndex < numNeighbors) {
            Tree *current = neighbors[queueIndex];
            queueIndex++;

            Tree **subNeighbors = (Tree **) malloc(sizeof(Tree *));
            if (subNeighbors == NULL)
                continue;

            int subCount = 0;

            findNeighborsInRadius(root, current->point, root->dim, epsSquared, &subNeighbors, &subCount);

            /* Если current является core point */
            if (subCount >= minPts) {
                for (int j = 0; j < subCount; j++) {
                    Tree *candidate = subNeighbors[j];

                    /* Если точка ещё не классифицирована */
                    if (candidate->clusterIdx == UNCLASSIFIED) {
                        candidate->clusterIdx = currentCluster;

                        Tree **tmp = realloc(neighbors, (numNeighbors + 1) * sizeof(Tree *));

                        if (tmp != NULL) {
                            neighbors = tmp;
                            neighbors[numNeighbors] = candidate;
                            numNeighbors++;
                        }
                    }
                    /* Если была шумом -> включаем в кластер */
                    else if (candidate->clusterIdx == NOISE) {
                        candidate->clusterIdx = currentCluster;
                    }
                }
            }

            free(subNeighbors);
        }

        free(neighbors);
    }

    free(allNodes);

    return currentCluster;
}
