#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kdTree.c"

#define COUNT_OF_ARGUMENTS 2
#define SEPARATOR ','

int main(int argc, char **argv) {
    // ===== OPEN FILE =====
    /*
    if (argc != COUNT_OF_ARGUMENTS) {
        printf("Not enough arguments provided\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "r");
    */
    FILE *file = fopen("lidar.csv", "r");

    if (file == NULL) {
        printf("Ошибка при открытии файла!\n");
        return 1;
    }

    // ===== FIND COUNT OF LINE IN FILE =====

    int lines = 0;
    int ch;

    // Читаем посимвольно до конца файла
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }

    // ===== READ POINTS =====
    // char *line = malloc(256 * sizeof(char));
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

    file = fopen("lidar.csv", "r");

    double **points = malloc(lines * sizeof(double *));
    // while (fscanf(file, "%s", line) == 1) {
    // }

    for (int i = 0; i < lines; i++) {
        fscanf(file, "%s", line);
        points[i] = malloc(sizeof(double) * dim);
        token = strtok(line, ",");
        for (int j = 0; j < dim; j++) {
            points[i][j] = atof(token);
            token = strtok(NULL, ",");
        }
    }

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < dim; j++) {
            printf("%lf ", points[i][j]);
        }
        printf("\n");
    }

    free(token);
    // free(line);

    return 0;
}
