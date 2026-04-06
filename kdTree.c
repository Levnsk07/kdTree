#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Tree {
    int dim; // Размерность точек
    // int *point; // Координаты точки
    double *point; // Координаты точки
    int splitByDim; // Ось разделения
    struct Tree *right;
    struct Tree *left;
} Tree;

// ============== INIT/ADD POINT ======================
Tree *initKDTree(int dim) {
    Tree *tree = malloc(sizeof(Tree));
    tree->dim = dim;
    tree->splitByDim = 0;
    tree->point = malloc(sizeof(double) * dim);
    tree->right = NULL;
    tree->left = NULL;
    return tree;
}

static int sortDim; // with witch dim. need to sort

int compare(const void *a, const void *b) {
    // func for Qsort
    const int *pa = *(const int **) a;
    const int *pb = *(const int **) b;
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
    buildKDTree(&node->right, points, right, med + 1, dim, depth + 1);

    *tree = node;
}

// ============== GET POINT ======================

Tree *getPointInTree(Tree *tree, double *point, int dim) {
    // find point in tree
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
    // get min Point in this "line"
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

//      ============== get nearest ======================



// ============== REMOVE POINT ======================

Tree *deleteNode(Tree *tree, double *point, int dim) {
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
    } else {
        if (point[tree->splitByDim] < tree->point[tree->splitByDim]) {
            tree->left = deleteNode(tree->left, point, dim);
        } else {
            tree->right = deleteNode(tree->right, point, dim);
        }
    }
    return tree;
}

// ============== ELSE ======================
void printTree(Tree *node, int level) {
    // AI -- remove
    if (node == NULL) {
        return;
    }

    // Отступы для наглядности
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Печать информации об узле
    printf("Split by dim %d: (", node->splitByDim);
    for (int i = 0; i < node->dim; i++) {
        printf("%lf", node->point[i]);
        if (i < node->dim - 1) {
            printf(", ");
        }
    }
    printf(")\n");

    // Рекурсивный вызов для левого и правого поддерева
    printTree(node->left, level + 1);
    printTree(node->right, level + 1);
}

// Освобождение памяти
// void freeTree(Tree *node) {
//     if (node == NULL) {
//         return;
//     }
//     freeTree(node->left);
//     freeTree(node->right);
//     free(node->point);
//     free(node);
// }
