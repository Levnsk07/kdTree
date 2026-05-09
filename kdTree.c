#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "kdTree.h"

typedef struct Tree {
    int dim;
    double *point;
    int splitByDim;
    struct Tree *right;
    struct Tree *left;
    struct Tree *parent;

    int clasterID;
} Tree;

// ============== INIT ======================
static Tree *initKDTree(int dim) {
    Tree *tree = malloc(sizeof(Tree));
    tree->dim = dim;
    tree->splitByDim = 0;
    tree->point = malloc(sizeof(double) * dim);
    tree->right = NULL;
    tree->left = NULL;
    tree->parent = NULL;
    tree->clasterID = -1;

    return tree;
}

/// ====== Build


static int sortDim;

static int compare(const void *a, const void *b) {
    const double *pa = *(const double **) a;
    const double *pb = *(const double **) b;
    if (pa[sortDim] < pb[sortDim]) return -1;
    if (pa[sortDim] > pb[sortDim]) return 1;
    return 0;
}

static bool pointsEqual(double *point1, double *point2, int dim) {
    bool isEqual = true;
    for (int i = 0; i < dim; i++) {
        if (point1[i] != point2[i]) {
            isEqual = false;
            break;
        }
    }
    return isEqual;
}

static void buildKDTreeAPI(Tree **tree, double **points, int right, int left, int dim, int depth) {
    if (right <= left) {
        *tree = NULL;
        return;
    }

    sortDim = depth % dim;
    qsort(points + left, right - left, sizeof(double *), compare);
    int med = left + (right - left) / 2;

    Tree *node = initKDTree(dim);
    node->point = points[med];
    node->splitByDim = sortDim;

    buildKDTreeAPI(&node->left, points, med, left, dim, depth + 1);
    if (node->left != NULL) {
        node->left->parent = node;
    }

    buildKDTreeAPI(&node->right, points, right, med + 1, dim, depth + 1);
    if (node->right != NULL) {
        node->right->parent = node;
    }

    *tree = node;
}

Tree *buildKdTree(double **points, int right, int dim) {
    Tree *tree = initKDTree(dim);
    buildKDTreeAPI(&tree, points, right, 0, dim, 0);
    return tree;
}

// ============== GET POINT ======================

Tree *getPointInTree(Tree *tree, double *point, int dim) {
    if (tree == NULL) {
        return NULL;
    }

    int isEqual = 1;
    for (int i = 0; i < dim; i++) {
        if (tree->point[i] != point[i]) {
            isEqual = 0;
            break;
        }
    }

    if (isEqual) {
        return tree;
    }
    Tree *foundInLeft = getPointInTree(tree->left, point, dim);
    if (foundInLeft != NULL) {
        return foundInLeft;
    }
    Tree *foundInRight = getPointInTree(tree->right, point, dim);
    if (foundInRight != NULL) {
        return foundInRight;
    }
    return NULL;
}

static Tree *findMin(Tree *tree, int dimToCompare, int dim) {
    if (tree == NULL) {
        return NULL;
    }
    if (tree->splitByDim == dimToCompare) {
        if (tree->left == NULL) {
            return tree;
        }
        return findMin(tree->left, dimToCompare, dim);
    }
    Tree *leftMin = findMin(tree->left, dimToCompare, dim);
    Tree *rightMin = findMin(tree->right, dimToCompare, dim);
    Tree *minNode = tree;
    if (leftMin != NULL && leftMin->point[dimToCompare] < minNode->point[dimToCompare]) {
        minNode = leftMin;
    }
    if (rightMin != NULL && rightMin->point[dimToCompare] < minNode->point[dimToCompare]) {
        minNode = rightMin;
    }
    return minNode;
}

static Tree *findMax(Tree *tree, int dimToCompare, int dim) {
    if (tree == NULL) {
        return NULL;
    }

    if (tree->splitByDim == dimToCompare) {
        if (tree->right == NULL) {
            return tree;
        }
        return findMax(tree->right, dimToCompare, dim);
    }

    Tree *leftMax = findMax(tree->left, dimToCompare, dim);
    Tree *rightMax = findMax(tree->right, dimToCompare, dim);
    Tree *maxNode = tree;

    if (leftMax != NULL && leftMax->point[dimToCompare] > maxNode->point[dimToCompare]) {
        maxNode = leftMax;
    }
    if (rightMax != NULL && rightMax->point[dimToCompare] > maxNode->point[dimToCompare]) {
        maxNode = rightMax;
    }
    return maxNode;
}

// ============== FIND NEAREST ======================

static double distance(double *point1, double *point2, int dim) {
    double dist = 0.0;
    for (int i = 0; i < dim; i++) {
        double diff = point1[i] - point2[i];
        dist += diff * diff;
    }
    return dist;
}

/*
static void findNearestRecursive(Tree *tree, double *point, int dim, Tree **nearest, double *minDistance,
                                 Tree *exclude) {
    if (tree == NULL || tree == exclude) {
        return;
    }

    double currentDistance = distance(tree->point, point, dim);


    if (currentDistance < *minDistance) {
        *minDistance = currentDistance;
        *nearest = tree;
    }

    int axis = tree->splitByDim;
    if (point[axis] < tree->point[axis]) {
        findNearestRecursive(tree->left, point, dim, nearest, minDistance, exclude);

        if (pow(point[axis] - tree->point[axis], 2) < *minDistance) {
            // right tree
            findNearestRecursive(tree->right, point, dim, nearest, minDistance, exclude);
        }
    } else {
        findNearestRecursive(tree->right, point, dim, nearest, minDistance, exclude);
        if (pow(point[axis] - tree->point[axis], 2) < *minDistance) {
            // left tree
            findNearestRecursive(tree->left, point, dim, nearest, minDistance, exclude);
        }
    }
}


double *findNearest(Tree *root, double *point, int dim) {
    if (root == NULL) {
        return NULL;
    }

    Tree *nearest = NULL;
    double minDistance = INFINITY;
    // check is point exist
    Tree *exclude = getPointInTree(root, point, dim);
    if (exclude == NULL) {
        printf("Point in tree not found \n");
        return NULL;
    };

    findNearestRecursive(root, point, dim, &nearest, &minDistance, exclude);

    return nearest->point;
}*/

static void findNearestRecursive(Tree *tree, double *point, int dim, Tree **nearest, double *minDistance,
                                 Tree *exclude) {
    if (tree == NULL || tree == exclude) {
        return;
    }

    double currentDistance = distance(tree->point, point, dim);


    if (currentDistance < *minDistance) {
        *minDistance = currentDistance;
        *nearest = tree;
    }

    int axis = tree->splitByDim;
    double diff = point[axis] - tree->point[axis];
    if (diff < 0) {
        findNearestRecursive(tree->left, point, dim, nearest, minDistance, exclude);
        if (diff * diff < *minDistance) {
            // right tree
            findNearestRecursive(tree->right, point, dim, nearest, minDistance, exclude);
        }
    } else {
        findNearestRecursive(tree->right, point, dim, nearest, minDistance, exclude);
        if (diff * diff < *minDistance) {
            // left tree
            findNearestRecursive(tree->left, point, dim, nearest, minDistance, exclude);
        }
    }
}


// убрал строку, теперь можно искать ближайшую точку к произвольной
// тогда если точка уже существует, то она не будет учитываться при поиске
Tree *findNearest(Tree *root, double *point, int dim) {
    if (root == NULL) {
        return NULL;
    }

    Tree *nearest = NULL;
    double minDistance = INFINITY;
    Tree *exclude = getPointInTree(root, point, dim);

    findNearestRecursive(root, point, dim, &nearest, &minDistance, exclude);

    return nearest;
}

// ============== ADD POINT ======================

static void copyPoint(double *pointDst, double *pointSrc, int dim) {
    for (int i = 0; i < dim; i++)
        pointDst[i] = pointSrc[i];
}

static Tree *createNode(double *point, int dim, int splitByDim, Tree *parent) {
    Tree *newNode = (Tree *) malloc(sizeof(Tree));
    newNode->dim = dim;
    newNode->point = (double *) malloc(dim * sizeof(double));
    copyPoint(newNode->point, point, dim);
    newNode->splitByDim = splitByDim;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = parent;

    return newNode;
}

static Tree *insertPointRecursive(Tree *root, double *point, int dim, int splitByDim, Tree *parent) {
    if (root == NULL)
        return createNode(point, dim, splitByDim, parent);

    if (pointsEqual(root->point, point, dim))
        return root;

    int axis = root->splitByDim;
    int nextSplitByDim = (axis + 1) % dim;

    if (point[axis] < root->point[axis])
        root->left = insertPointRecursive(root->left, point, dim, nextSplitByDim, root);
    else
        root->right = insertPointRecursive(root->right, point, dim, nextSplitByDim, root);

    return root;
}


Tree *insertPoint(Tree *root, double *point, int dim) {
    return insertPointRecursive(root, point, dim, 0, NULL);
}


// ============== REMOVE POINT ======================
Tree *deleteNodeV2(Tree *root, double *point, int dim){
    if(root == NULL)
        return NULL;

    if(pointsEqual(root->point, point, dim)){
        if(root->right != NULL){
            Tree *minNode = findMin(root->right, root->splitByDim, dim);
            copyPoint(root->point, minNode->point, dim);
            root->right = deleteNodeV2(root->right, minNode->point, dim);
            if(root->right != NULL)
                root->right->parent = root;
        }
        else if(root->left != NULL){
            Tree *maxNode = findMax(root->left, root->splitByDim, dim);
            copyPoint(root->point, maxNode->point, dim);
            root->left = deleteNodeV2(root->left, maxNode->point, dim);
            if(root->left != NULL)
                root->left->parent = root;
        }
        else{
            free(root->point);
            free(root);
            return NULL;
        }

        return root;
    }

    int axis = root->splitByDim;
    if(point[axis] < root->point[axis]){
        root->left = deleteNodeV2(root->left, point, dim);
        if(root->left != NULL)
            root->left->parent = root;
    }
    else if(point[axis] > root->point[axis]){
        root->right = deleteNodeV2(root->right, point, dim);
        if(root->right != NULL)
            root->right->parent = root;
    }
    else{
        root->left = deleteNodeV2(root->left, point, dim);
        if(root->left != NULL)
            root->left->parent = root;

        root->right = deleteNodeV2(root->right, point, dim);
        if(root->right != NULL)
            root->right->parent = root;
    }

    return root;
}




Tree *deleteNodeV1(Tree *root, double *point, int dim) {
    Tree *tree = getPointInTree(root, point, dim);

    if (tree == NULL) {
        return NULL;
    }

    if (tree->right != NULL) {
        Tree *minNode = findMin(tree->right, tree->splitByDim, dim);

        for (int i = 0; i < dim; i++) {
            tree->point[i] = minNode->point[i];
        }
        tree->right = deleteNodeV1(tree->right, minNode->point, dim);
    } else if (tree->left != NULL) {
        Tree *maxNode = findMax(tree->left, tree->splitByDim, dim);

        for (int i = 0; i < dim; i++) {
            tree->point[i] = maxNode->point[i];
        }

        tree->left = deleteNodeV1(tree->left, maxNode->point, dim);
    } else {
        free(tree);
        return NULL;
    }
    return tree;
}

// ============== FREE TREE ======================
void freeKDtree(Tree *tree) {
    if (tree == NULL)
        return;
    freeKDtree(tree->left);
    freeKDtree(tree->right);
    free(tree->point);
    free(tree);
}


// ============== ELSE ======================
void printTree(Tree *node, int level) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    printf("Split by dim %d: (", node->splitByDim);
    for (int i = 0; i < node->dim; i++) {
        printf("%lf", node->point[i]);
        if (i < node->dim - 1) {
            printf(", ");
        }
    }
    printf(")\n");

    printTree(node->left, level + 1);
    printTree(node->right, level + 1);
}
