#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "kdTree.h"


// подключил заголовк, убрал лишнее определение структуры дерева


static int sortDim = 0;

// ============== STATIC FUNC =========================

static int compare(const void *a, const void *b) {
    const double *pa = *(const double **) a;
    const double *pb = *(const double **) b;
    if(pa[sortDim] < pb[sortDim]) return -1;
    if(pa[sortDim] > pb[sortDim]) return 1;
    return 0;
}

static bool pointsEqual(double *point1, double *point2, int dim){
    bool isEqual = true;
    for (int i = 0; i < dim; i++) {
        if (point1[i] != point2[i]) {
            isEqual = false;
            break;
        }
    }
    return isEqual;
}

static void copyPoint(double *pointDst, double *pointSrc, int dim){
    for(int i=0; i < dim; i++)
        pointDst[i] = pointSrc[i];
}

static Tree *createNode (double *point, int dim, int splitByDim, Tree *parent){
    Tree *newNode = (Tree*)malloc(sizeof(Tree));
    newNode->dim = dim;
    newNode->point = (double*)malloc(dim*sizeof(double));
    copyPoint(newNode->point, point, dim);
    newNode->splitByDim = splitByDim;
    newNode->clusterIdx = -1;
    newNode->memberShip = NULL;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = parent;

    return newNode;
}

// ================= BUILD K-D TREE ============================

static void buildKDTreeRecursive(Tree **tree, double **points, int right, int left, int dim, int depth) {
    if (right <= left) {
        *tree = NULL;
        return;
    }

    sortDim = depth % dim;
    qsort(points + left, right - left, sizeof(double *), compare);
    int med = left + (right - left) / 2;

    Tree *node = createNode(points[med], dim, sortDim, NULL);

    buildKDTreeRecursive(&node->left, points, med, left, dim, depth + 1);
    if (node->left != NULL) {
        node->left->parent = node;
    }

    buildKDTreeRecursive(&node->right, points, right, med + 1, dim, depth + 1);
    if (node->right != NULL) {
        node->right->parent = node;
    }

    *tree = node;
}

Tree *buildKDTree(double **points, int count, int dim){
    Tree *tree = NULL;

    if(points == NULL || count <= 0 || dim <= 0)
        return NULL;
    
    buildKDTreeRecursive(&tree, points, count, 0, dim, 0);

    return tree;
}

// ============== ADD POINT ======================

// добавил отдельную функцию для добавления точки в K-D дерево

static Tree *insertPointRecursive(Tree *root, double *point, int dim, int splitByDim, Tree *parent){
    if(root == NULL)
        return createNode(point, dim, splitByDim, parent);

    if(pointsEqual(root->point, point, dim))
        return root;

    int axis = root->splitByDim;
    int nextSplitByDim = (axis + 1) % dim;

    if(point[axis] < root->point[axis])
        root->left = insertPointRecursive(root->left, point, dim, nextSplitByDim, root);
    else
        root->right = insertPointRecursive(root->right, point, dim, nextSplitByDim, root);

    return root;
}


Tree *insertPoint(Tree *root, double *point, int dim){
    return insertPointRecursive(root, point, dim, 0, NULL);
}

// ============== GET POINT ======================
// чуть переписал, тк до этого работал за O(n), как DFS. Теперь корректно
Tree *getPointInTree(Tree *tree, double *point, int dim) {
    if (tree == NULL) {
        return NULL;
    }

    if (pointsEqual(tree->point, point, dim)) {
        return tree;
    }

    int axis = tree->splitByDim;

    if(point[axis] < tree->point[axis])
        return getPointInTree(tree->left, point, dim);
    else if(point[axis] > tree->point[axis])
        return getPointInTree(tree->right, point, dim);
    else{
        Tree *foundInLeft = getPointInTree(tree->left, point, dim);
        if (foundInLeft != NULL) {
            return foundInLeft;
        }
        Tree *foundInRight = getPointInTree(tree->right, point, dim);
        if (foundInRight != NULL) {
            return foundInRight;
        }
    }
    return NULL;
}

Tree *findMin(Tree *tree, int dimToCompare, int dim) {
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

Tree *findMax(Tree *tree, int dimToCompare, int dim) {
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

double distancePoint(double *point1, double *point2, int dim) {
    double dist = 0.0, diff = 0.0;
    for (int i = 0; i < dim; i++) {
        diff = point1[i] - point2[i];
        dist += diff * diff;
    }
    return sqrt(dist);
}

static void findNearestRecursive(Tree *tree, double *point, int dim, Tree **nearest, double *minDistance, Tree *exclude) {
    if (tree == NULL || tree == exclude) {
        return;
    }

    double currentDistance = distancePoint(tree->point, point, dim);


    if (currentDistance < *minDistance) {
        *minDistance = currentDistance;
        *nearest = tree;
    }

    int axis = tree->splitByDim;
    double diff = point[axis] - tree->point[axis];
    if (diff < 0) {
        findNearestRecursive(tree->left, point, dim, nearest, minDistance, exclude);
        if (fabs(diff) < *minDistance) {
            // right tree
            findNearestRecursive(tree->right, point, dim, nearest, minDistance, exclude);
        }
    } else {
        findNearestRecursive(tree->right, point, dim, nearest, minDistance, exclude);
        if (fabs(diff) < *minDistance) {
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


// ============== REMOVE POINT ======================

Tree *deleteNode(Tree *root, double *point, int dim){
    if(root == NULL)
        return NULL;

    if(pointsEqual(root->point, point, dim)){
        if(root->right != NULL){
            Tree *minNode = findMin(root->right, root->splitByDim, dim);
            copyPoint(root->point, minNode->point, dim);
            root->right = deleteNode(root->right, minNode->point, dim);
            if(root->right != NULL)
                root->right->parent = root;
        }
        else if(root->left != NULL){
            Tree *maxNode = findMax(root->left, root->splitByDim, dim);
            copyPoint(root->point, maxNode->point, dim);
            root->left = deleteNode(root->left, maxNode->point, dim);
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
        root->left = deleteNode(root->left, point, dim);
        if(root->left != NULL)
            root->left->parent = root;
    }
    else if(point[axis] > root->point[axis]){
        root->right = deleteNode(root->right, point, dim);
        if(root->right != NULL)
            root->right->parent = root;
    }
    else{
        root->left = deleteNode(root->left, point, dim);
        if(root->left != NULL)
            root->left->parent = root;

        root->right = deleteNode(root->right, point, dim);
        if(root->right != NULL)
            root->right->parent = root;
    }

    return root;
}


// ============== FREE TREE ======================
void freeKDtree(Tree *tree) {
    if (tree == NULL)
        return;
    freeKDtree(tree->left);
    freeKDtree(tree->right);
    if(tree->memberShip != NULL)
        free(tree->memberShip);
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
