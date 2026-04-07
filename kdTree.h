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

Tree *initKDTree(int dim);

int compare(const void *a, const void *b);

void buildKDTree(Tree **tree, double **points, int right, int left, int dim, int depth);

Tree *getPointInTree(Tree *tree, double *point, int dim);

Tree *findMin(Tree *tree, int dimToCompare, int dim);

Tree *findMax(Tree *tree, int dimToCompare, int dim);

double distance(double *point1, double *point2, int dim);

void findNearestRecursive(Tree *tree, double *point, int dim, Tree **nearest, double *minDistance, Tree *exclude);

Tree *findNearest(Tree *root, double *point, int dim);

Tree *deleteNode(Tree *root, double *point, int dim);

void printTree(Tree *node, int level);

#endif