#include <stdio.h>
#include <stdlib.h>
#include "kdTree.h"

void printTreeClusters(Tree *root, int clustersCount, int level){
    if(root == NULL)
        return;
    
    for(int i=0; i < root->dim; i++)
        printf(" ");
    
    printf("Point: (");
    for(int i=0; i < root->dim; i++){
        printf("%lf", root->point[i]);
        if(i < root->dim-1)
            printf(", ");
    }

    printf("), cluster = %d, membership = [", root->clusterIdx);

    for(int i=0; i < clustersCount; i++){
        printf("%lf", root->memberShip[i]);
        if(i < clustersCount-1)
            printf(", ");
    }

    printf("]\n");
    printTreeClusters(root->left, clustersCount, level+1);
    printTreeClusters(root->right, clustersCount, level+1);
}

int main() {
    int dim = 2;
    double pointsArray[][2] = {{1, 1}, {1, 2}, {2, 1},
                                {8, 8}, {8, 9}, {9, 8},
                                {20, 20}, {21, 20}, {20, 21}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);

    double **points = (double**)malloc(count * sizeof(double*));
    for (int i = 0; i < count; i++) {
        points[i] = pointsArray[i];
    }

    Tree *tree = buildKDTree(points, count, dim);

    printf("KD-Tree:\n");
    printTree(tree, 0);
    // double point[2]={2,7};

    printf("====================\n");
    fuzzyCMeans(tree, count, dim, 3);
    printTreeClusters(tree, 3, 0);

    free(points);
    freeKDtree(tree);
    

    return 0;
}


