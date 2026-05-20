#ifndef K_DTREE_KDTREE_H
#define K_DTREE_KDTREE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Структура узла k-d дерева
typedef struct Tree {
    int dim;
    double *point;
    int splitByDim;
    struct Tree *right;
    struct Tree *left;
    struct Tree *parent;

    int clusterIdx;
    double *memberShip;
} Tree;

/// ============== Build K-D tree
Tree *buildKDTree(double **points, int count, int dim);

/// ============== Get Point
Tree *getPointInTree(Tree *tree, double *point, int dim);

/// ============== distance between points (вопрос, оставлять или нет)
double distancePoint(double *point1, double *point2, int dim);

/// ============== Insert point
Tree *insertPoint(Tree *root, double *point, int dim);

// =============== Find nearest point
Tree *findNearest(Tree *root, double *point, int dim);

/// ============== Remove Point 
Tree *deleteNode(Tree *root, double *point, int dim);

/// ============== Free Tree
void freeKDtree(Tree *tree);

/// ============== Else functions
void printTree(Tree *node, int level);

/// ============== Clustering
int dbscanCluster(Tree *root, double eps, int minPts, int count);

bool fuzzyCMeans(Tree *root, int pointsCount, int dim, int clustersCount);


#endif //K_DTREE_KDTREE_H
