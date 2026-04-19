#ifndef K_DTREE_KDTREE_H
#define K_DTREE_KDTREE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Структура узла k-d дерева
typedef struct Tree {
    int dim;
    double *point;
    int splitByDim;
    struct Tree *right;
    struct Tree *left;
    struct Tree *parent;
} Tree;

// Структура для хранения результатов Fuzzy C-means
typedef struct {
    double **centroids;
    double **memberships;
    int num_clusters;
    int num_points;
    int dim;
} FuzzyCMeansResult;

// ===== k-d Tree Functions =====
Tree *initKDTree(int dim);
void buildKDTree(Tree **tree, double **points, int right, int left, int dim, int depth);
Tree *getPointInTree(Tree *tree, double *point, int dim);
Tree *findMin(Tree *tree, int dimToCompare, int dim);
Tree *findMax(Tree *tree, int dimToCompare, int dim);
double distance(double *point1, double *point2, int dim);
void findNearestRecursive(Tree *tree, double *point, int dim, Tree **nearest, double *minDistance, Tree *exclude);
Tree *findNearest(Tree *root, double *point, int dim);
Tree *deleteNode(Tree *root, double *point, int dim);
void printTree(Tree *node, int level);

// ===== Clustering Functions =====
FuzzyCMeansResult fuzzyCMeans(double **points, int num_points, int dim, int num_clusters, double m, int max_iter);
void freeFuzzyCMeansResult(FuzzyCMeansResult *result);
void dbscan(Tree *tree, int dim, double eps, int minPts, int *clusterIds, int *clusterCount, int numPoints, double **points);
void writePointsToFile(const char *filename, double **points, int *clusterIds, int num_points, int dim, int clusterId);

#endif //K_DTREE_KDTREE_H