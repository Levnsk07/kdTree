#include <stdio.h>
#include <stdlib.h>
#include "kdTree.c"

int main() {
    int dim = 2;
    double pointsArray[][2] = {{3, 6}, {17, 15}, {13, 15}, {6, 12}, {9, 1}, {2, 7}, {10, 19}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);

    double **points = malloc(count * sizeof(int*));
    for (int i = 0; i < count; i++) {
        points[i] = pointsArray[i];
    }

    Tree *tree;
    buildKDTree(&tree, points, count, 0, dim, 0);

    printf("KD-Tree:\n");
    printTree(tree, 0);
    double point[2]={2,7};

    printf("====================\n");
    deleteNode(tree,point,2);
    printTree(tree,0);

    

    return 0;
}


