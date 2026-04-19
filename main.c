#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "kdTree.c"
#include "clustering.c"

#define COUNT_OF_ARGUMENTS_MIN 3
#define SEPARATOR ','


int main(int argc, char **argv) {
    FILE *file = fopen(argv[1], "r");

    if (file == NULL) {
        printf("Ошибка при открытии файла!\n");
        return 1;
    }

    // ===== FIND COUNT OF LINES IN FILE =====
    int lines = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }

    // ===== READ POINTS =====

    char line[256];
    int dim = 1;

    // get dim
    double *arr = malloc(sizeof(double) * 100);
    fscanf(file, "%s", line);
    char *token = strtok(line, ",");
    while (token != NULL) {
        arr[dim++] = atof(token);
        token = strtok(NULL, ",");
    }
    free(arr);

    file = fopen(argv[1], "r");

    double **points = malloc(lines * sizeof(double *));

    for (int i = 0; i < lines; i++) {
        fscanf(file, "%s", line);
        points[i] = malloc(sizeof(double) * dim);
        token = strtok(line, ",");
        for (int j = 0; j < dim; j++) {
            points[i][j] = atof(token);
            token = strtok(NULL, ",");
        }
    }
    free(token);

    Tree *tree = initKDTree(dim);
    buildKDTree(&tree, points, lines, 0, dim, 0);
    free(points);


    // ==========   COMMANDS

    if (strcmp(argv[2], "-kd_insert") == 0) {
        FILE *wrighter = fopen(argv[1], "a");
        fprintf(wrighter, "%s", argv[3]);
        return 0;
    } else if (strcmp(argv[2], "-kd_nearest") == 0) {
        double *point = malloc(sizeof(double) * dim);
        char *line = argv[3];

        char *token = strtok(line, ",");
        int temp = 0;
        while (token != NULL) {
            point[temp] = atof(token);
            token = strtok(NULL, ",");
            temp++;
        }
        double *nearest = findNearest(tree, point, dim);

        for (int i = 0; i < dim; i++) {
            printf("%lf ", nearest[i]);
        }
        printf("\n");
    }


    return 0;
}
