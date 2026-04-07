#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Tree {
    int dim;
    double *point;
    int splitByDim;
    struct Tree *right;
    struct Tree *left;
    struct Tree *parent;
} Tree;

// ============== INIT/ADD POINT ======================
Tree *initKDTree(int dim) {
    Tree *tree = malloc(sizeof(Tree));
    tree->dim = dim;
    tree->splitByDim = 0;
    tree->point = malloc(sizeof(double) * dim);
    tree->right = NULL;
    tree->left = NULL;
    tree->parent = NULL;
    return tree;
}

static int sortDim;

int compare(const void *a, const void *b) {
    const double *pa = *(const double **) a;
    const double *pb = *(const double **) b;
    return pa[sortDim] - pb[sortDim];
}

void buildKDTree(Tree **tree, double **points, int right, int left, int dim, int depth) {
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

    buildKDTree(&node->left, points, med, left, dim, depth + 1);
    if (node->left != NULL) {
        node->left->parent = node;
    }

    buildKDTree(&node->right, points, right, med + 1, dim, depth + 1);
    if (node->right != NULL) {
        node->right->parent = node;
    }

    *tree = node;
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

double distance(double *point1, double *point2, int dim) {
    double dist = 0.0;
    for (int i = 0; i < dim; i++) {
        double diff = point1[i] - point2[i];
        dist += diff * diff;
    }
    return dist;
}

void findNearestRecursive(Tree *tree, double *point, int dim, Tree **nearest, double *minDistance, Tree *exclude) {
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

Tree *findNearest(Tree *root, double *point, int dim) {
    if (root == NULL) {
        return NULL;
    }

    Tree *nearest = NULL;
    double minDistance = INFINITY;
    // check is point exist
    Tree *exclude = getPointInTree(root, point, dim);
    if (exclude == NULL) return NULL;

    findNearestRecursive(root, point, dim, &nearest, &minDistance, exclude);

    return nearest;
}


// ============== REMOVE POINT ======================
Tree *deleteNode(Tree *root, double *point, int dim) {
    Tree *tree = getPointInTree(root, point, dim);

    if (tree == NULL) {
        return NULL;
    }

    if (tree->right != NULL) {
        Tree *minNode = findMin(tree->right, tree->splitByDim, dim);

        for (int i = 0; i < dim; i++) {
            tree->point[i] = minNode->point[i];
        }
        tree->right = deleteNode(tree->right, minNode->point, dim);
    } else if (tree->left != NULL) {
        Tree *maxNode = findMax(tree->left, tree->splitByDim, dim);

        for (int i = 0; i < dim; i++) {
            tree->point[i] = maxNode->point[i];
        }

        tree->left = deleteNode(tree->left, maxNode->point, dim);
    } else {
        free(tree);
        return NULL;
    }
    return tree;
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
