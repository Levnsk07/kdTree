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

static double distance(double *a, double *b, int dim);

static Tree *initKDTree(int dim);

static int compare(const void *a, const void *b);

static void buildKDTreeAPI(Tree **tree, double **points, int right, int left, int dim, int depth);

Tree *buildKdTree(double **points, int right, int dim);

Tree *getPointInTree(Tree *tree, double *point, int dim);

static Tree *findMin(Tree *tree, int dimToCompare, int dim);

static Tree *findMax(Tree *tree, int dimToCompare, int dim);

static void findNearestRecursive(Tree *tree, double *point, int dim, Tree **nearest, double *minDistance, Tree *exclude);

// Tree *findNearest(Tree *root, double *point, int dim);
double *findNearest(Tree *root, double *point, int dim);

Tree *deleteNode(Tree *root, double *point, int dim);

void printTree(Tree *node, int level);


// Clusters
int *dbscan(Tree *root, double **points, int numPoints, int dim, double eps, int minPts);


#endif //K_DTREE_KDTREE_H
