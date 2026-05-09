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

    int clasterID;
} Tree;

/// ============== Build
Tree *buildKdTree(double **points, int right, int dim);

/// ============== Get Point
Tree *getPointInTree(Tree *tree, double *point, int dim);

// double *findNearest(Tree *root, double *point, int dim);
Tree *findNearest(Tree *root, double *point, int dim);

/// ============== Add/Remove Point
Tree *deleteNodeV1(Tree *root, double *point, int dim); // TODO Test and choose only one
Tree *deleteNodeV2(Tree *root, double *point, int dim);

Tree *insertPoint(Tree *root, double *point, int dim);

/// ============== Free Tree
void freeKDtree(Tree *tree);

/// ============== Else functions

void printTree(Tree *node, int level);


/// ============== Clustering

int *dbscan(Tree *root, double **points, int numPoints, int dim, double eps, int minPts);


#endif //K_DTREE_KDTREE_H
