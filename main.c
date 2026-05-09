#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// #include "kdTree.c"
#include "kdTree.c"
#include "clustering.c"

#define COUNT_OF_ARGUMENTS_MIN 3
#define SEPARATOR ','

void writePointsToFiles(double **points, int *clusterIds, int num_points, int dim, int maxClusterId,
                        const char *base_filename) {
    for (int clusterId = 1; clusterId <= maxClusterId; clusterId++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "./%s_%d.csv", base_filename, clusterId);
        printf("Попытка создания файла: %s\n", filename); // Отладочное сообщение
        FILE *file = fopen(filename, "w");
        if (!file) {
            printf("Ошибка при создании файла: %s\n", filename);
            continue;
        }

        // Запись заголовка
        for (int i = 0; i < dim; i++) {
            fprintf(file, "dim%d", i + 1);
            if (i < dim - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");

        // Запись точек, принадлежащих кластеру
        for (int i = 0; i < num_points; i++) {
            if (clusterIds[i] == clusterId) {
                for (int j = 0; j < dim; j++) {
                    fprintf(file, "%lf", points[i][j]);
                    if (j < dim - 1) fprintf(file, ",");
                }
                fprintf(file, "\n");
            }
        }
        fclose(file);
        printf("Файл %s создан успешно!\n", filename); // Отладочное сообщение
    }
}

int main(int argc, char **argv) {
    if (argc < COUNT_OF_ARGUMENTS_MIN) {
        // TODO
        printf("Usage: %s <file.csv> <-kd_insert|-kd_nearest|-dbscan|-cmeans> <args>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r"); // TODO
    if (file == NULL) {
        printf("Ошибка при открытии файла!\n");
        return 1;
    }

    // ===== FIND COUNT OF LINES IN FILE =====
    int lines = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') lines++;
    }
    rewind(file);

    // ===== READ POINTS =====
    char line[256];
    int dim = 0;

    // get dim
    if (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        while (token != NULL) {
            dim++;
            token = strtok(NULL, ",");
        }
    }
    rewind(file);

    double **points = malloc(lines * sizeof(double *));
    for (int i = 0; i < lines; i++) {
        points[i] = malloc(sizeof(double) * dim);
    }

    for (int i = 0; i < lines; i++) {
        if (fgets(line, sizeof(line), file)) {
            char *token = strtok(line, ",");
            for (int j = 0; j < dim; j++) {
                points[i][j] = atof(token);
                token = strtok(NULL, ",");
            }
        }
    }
    fclose(file);

    // ===== BUILD k-d TREE =====
    Tree *tree = buildKdTree(points, lines, dim);

    // ======================== COMMANDS =======================================
    if (strcmp(argv[2], "-kd_insert") == 0) {
        FILE *writer = fopen(argv[1], "a");

        if (!writer) {
            printf("Ошибка при открытии файла для записи!\n");
            return 1;
        }

        fprintf(writer, "\n%s", argv[3]);
        fclose(writer);
        printf("Точка добавлена в файл: %s\n", argv[1]);
    } else if (strcmp(argv[2], "-kd_nearest") == 0) {
        double *point = malloc(sizeof(double) * dim);
        if (!point) {
            printf("Ошибка при выделении памяти!\n");
            return 1;
        }

        char *linePoint = argv[3];
        char *token = strtok(linePoint, ",");
        int temp = 0;
        while (token != NULL && temp < dim) {
            point[temp] = atof(token);
            token = strtok(NULL, ",");
            temp++;
        }

        double *nearest = findNearest(tree, point, dim);
        if (!nearest) {
            // TODO надо ли искать только в уже существующих?
            printf("Ошибка при поиске ближайшей точки!\n");
            free(point);
            return 1;
        }

        printf("Nearest Point: ");
        for (int i = 0; i < dim; i++) {
            printf("%lf ", nearest[i]);
        }
        printf("\n");
        free(point);
        free(nearest);
    } else if (strcmp(argv[2], "-dbscan") == 0) {
        char *args = argv[3];
        char *token = strtok(args, ",");
        double eps = atof(token);
        token = strtok(NULL, ",");
        int minPts = atoi(token);

        int *clusterIds = dbscan(tree, points, lines, dim, eps, minPts);
        if (!clusterIds) {
            printf("Ошибка при выполнении DBSCAN!\n");
            return 1;
        }

        for (int i = 0; i < lines; i++) {
            printf("%d: ", clusterIds[i]);
            for (int j = 0; j < dim; j++) {
                printf("%lf ", points[i][j]);
            }
            printf("\n");
        }


        int maxClusterId = 0;
        for (int i = 0; i < lines; i++) {
            if (clusterIds[i] > maxClusterId) {
                maxClusterId = clusterIds[i];
            }
        }

        // Сохранение результатов в файлы
        writePointsToFiles(points, clusterIds, lines, dim, maxClusterId, "dbscan");
        printf("Результаты DBSCAN сохранены в файлы: dbscan_X.csv\n");
        free(clusterIds);
    }


    // ============ FREE==============
    for (int i = 0; i < lines; i++) {
        free(points[i]);
    }
    free(points);
    return 0;
}
