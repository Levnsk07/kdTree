#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "kdTree.h"

#define COUNT_OF_ARGUMENTS_MIN 4
#define SEPARATOR ","
#define NOISE_CLUSTER -1
#define WRITE_CLUSTERS true
#define WRITE_POINTS false

void printAllFunc(){
    printf("All functions:\n");
    printf("    robot_spatial <file.csv> -kd_insert <x,y[,z]>\n");
    printf("    robot_spatial <file.csv> -kd_delete <x,y[,z]>\n");
    printf("    robot_spatial <file.csv> -kd_nearest <x,y[,z]>\n");
    printf("    robot_spatial <file.csv> -cmeans <clusters>\n");
    printf("    robot_spatial <file.csv> -dbscan <eps,minPts>\n");
}

bool parsePoint(const char *textPoints, double **point, int *dim){
    if(textPoints == NULL || point == NULL || dim == NULL)
        return false;

    char *buffer = (char*)malloc((strlen(textPoints) + 1)*sizeof(char));
    if(buffer == NULL)
        return false;
    strcpy(buffer, textPoints);

    int capacity=4, count=0;
    double *coords = (double*)malloc(capacity*sizeof(double));
    if(coords == NULL){
        free(buffer);
        return false;
    }

    char *token = strtok(buffer, SEPARATOR);

    char *endptr=NULL;
    double value=0.0;
    double *temp = NULL;
    while(token != NULL){
        if(count == capacity){
            capacity *= 2;
            temp = realloc(coords, capacity*sizeof(double));
            if(temp == NULL){
                free(coords);
                free(buffer);
                return false;
            }
            coords = temp;
        }

        value = strtod(token, &endptr);
        if(*endptr != '\0'){
            free(coords);
            free(buffer);
            return false;
        }

        coords[count] = value;
        count++;

        token = strtok(NULL, SEPARATOR);
    }

    free(buffer);
    
    if(count == 0){
        free(coords);
        return false;
    }

    *point = coords;
    *dim = count;
    return true;
}

bool parsePositiveInt(const char *text, int *value){
    if(text == NULL || value == NULL)
        return false;
    
    char *endptr = NULL;
    long parsed = strtol(text, &endptr, 10);

    if(*endptr != '\0' || parsed <= 0)
        return false;
    
    *value = (int)parsed;

    return true;
}

bool parseDbscanParams(const char *text, double *eps, int *minPts){
    if(text == NULL || eps == NULL || minPts == NULL)
        return false;

    char *buffer = (char*)malloc((strlen(text)+1)*sizeof(char));
    if(buffer == NULL)
        return false;
    
    strcpy(buffer, text);

    char *epsToken = strtok(buffer, SEPARATOR);
    char *minPtsToken = strtok(NULL, SEPARATOR);
    char *extraToken = strtok(NULL, SEPARATOR);

    if(epsToken == NULL || minPtsToken == NULL || extraToken != NULL){
        free(buffer);
        return false;
    }

    char *endptr = NULL;
    double epsParsed = strtod(epsToken, &endptr);

    if(*endptr != '\0' || epsParsed <= 0){
        free(buffer);
        return false;
    }

    int minPtsParsed = 0;

    if(!parsePositiveInt(minPtsToken, &minPtsParsed)){
        free(buffer);
        return false;
    }

    *eps = epsParsed;
    *minPts = minPtsParsed;

    free(buffer);
    return true;
}

void freePointsArray(double **points, int count){
    if(points == NULL)
        return;
    for(int i=0; i < count; i++)
        free(points[i]);
    free(points);
}

bool readPointsFromCSV(const char *filePath, double ***points, int *count, int *dim){
    if(filePath == NULL || points == NULL || count == NULL || dim == NULL)
        return false;
    FILE *input = fopen(filePath, "r");
    if(input == NULL)
        return false;

    int capacity=4, pointsCount=0, pointsDim=0;
    double **pointsFromCSV = (double**)malloc(capacity*sizeof(double*));
    if(pointsFromCSV == NULL){
        fclose(input);
        return false;
    }

    char line[1024];
    int len=0;

    double *point = NULL;
    int pointDim=0;
    double **temp = NULL;
    while(fgets(line, sizeof(line), input) != NULL){
        len = strcspn(line, "\r\n");
        line[len] = '\0';
    
        if(line[0] == '\0')
            continue;
        
        if(!parsePoint(line, &point, &pointDim)){
            freePointsArray(pointsFromCSV, pointsCount);
            fclose(input);
            return false;
        }

        if(pointsDim == 0)
            pointsDim = pointDim;
        else if(pointDim != pointsDim){
            free(point);
            freePointsArray(pointsFromCSV, pointsCount);
            fclose(input);
            return false;
        }

        if(pointsCount == capacity){
            capacity *= 2;
            temp = realloc(pointsFromCSV, capacity*sizeof(double*));
            if(temp == NULL){
                free(point);
                freePointsArray(pointsFromCSV, pointsCount);
                fclose(input);
                return false;
            }
            pointsFromCSV = temp;
        }

        pointsFromCSV[pointsCount] = point;
        pointsCount++;
    }
    
    if(pointsCount == 0){
        freePointsArray(pointsFromCSV, pointsCount);
        fclose(input);
        return false;
    }

    *points = pointsFromCSV;
    *count = pointsCount;
    *dim = pointsDim;

    fclose(input);
    return true;

}

void writePointsTreeToFile(FILE *file, Tree *root, int dim, int clusterIdx, bool clusters){
    if(root == NULL || file == NULL)
        return;
    
    if(!clusters){
        for(int i=0; i < dim; i++){
            fprintf(file, "%lf", root->point[i]);
            if(i < dim-1)
                fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    else if(clusters && root->clusterIdx == clusterIdx){
        for(int i=0; i < dim; i++){
            fprintf(file, "%lf", root->point[i]);
            if(i < dim-1)
                fprintf(file, ",");
        }
        fprintf(file, "\n");
    }

    writePointsTreeToFile(file, root->left, dim, clusterIdx, clusters);
    writePointsTreeToFile(file, root->right, dim, clusterIdx, clusters);
}

bool writeClustersToFiles(Tree *tree, int dim, int maxClusterIdx, const char *baseFileName){
    if(tree == NULL || dim <= 0 || maxClusterIdx <= 0 || baseFileName == NULL)
        return false;
    
    char fileName[256];
    for(int clusterIdx=0; clusterIdx < maxClusterIdx; clusterIdx++){
        snprintf(fileName, sizeof(fileName), "csv_files/%s_%d.csv", baseFileName, clusterIdx);
        printf("Attempt to create file: %s\n", fileName); // Отладочное сообщение
        FILE *file = fopen(fileName, "w");
        if (!file) {
            printf("Error: File %s was not created.\n", fileName);
            return false;
        }


        writePointsTreeToFile(file, tree, dim, clusterIdx, WRITE_CLUSTERS);

        fclose(file);
        printf("File %s was created successfully!\n", fileName); // Отладочное сообщение
    }

    if(strcmp(baseFileName, "dbscan") == 0){
        snprintf(fileName, sizeof(fileName), "csv_files/%s_noise.csv", baseFileName);
        FILE *file = fopen(fileName, "w");
        if (!file) {
            printf("Error: File %s was not created.\n", fileName);
            return false;
        }
        writePointsTreeToFile(file, tree, dim, NOISE_CLUSTER, WRITE_CLUSTERS);

        fclose(file);
        printf("File %s was created successfully!\n", fileName); // Отладочное сообщение
        
    }

    return true;
}

bool writePointsToFile(Tree *tree, int dim){
    if(tree == NULL || dim <= 0)
        return false;
    
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    char fileName[256];
    strftime(fileName, sizeof(fileName), "csv_files/points_%Y-%m-%d_%H-%M-%S.csv", local);

    printf("Attempt to create file: %s\n", fileName); // Отладочное сообщение
        FILE *file = fopen(fileName, "w");
        if (!file) {
            printf("Error: File %s was not created.\n", fileName);
            return false;
        }
    
    writePointsTreeToFile(file, tree, dim, 0, WRITE_POINTS);

    fclose(file);
    printf("File %s was created successfully!\n", fileName); // Отладочное сообщение
    
    return true;
}

int main(int argc, char **argv) {
    if (argc < COUNT_OF_ARGUMENTS_MIN) {
        printAllFunc();
        return 1;
    }

    char *filePath = argv[1];
    char *operation = argv[2];
    char *params = argv[3];

    double **pointsCSV = NULL;
    int countCSV=0, pointDimCSV=0;
    if(!readPointsFromCSV(filePath, &pointsCSV, &countCSV, &pointDimCSV)){
        printf("Error: Cannot read points from CSV.\n");
        return 1;
    }

    // ===== BUILD K-D TREE =====
    Tree *tree = buildKDTree(pointsCSV, countCSV, pointDimCSV);


    // ======================== COMMANDS =======================================
    if (strcmp(operation, "-kd_insert") == 0) {
        double *point = NULL;
        int pointDim = 0;

        if(!parsePoint(params, &point, &pointDim)){
            printf("Error: Invalid point format.\n");
            free(point);
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }
        else if(pointDim != pointDimCSV){
            printf("Error: Point dimension does not match CSV dimension.");
            free(point);
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

        Tree *found = getPointInTree(tree, point, pointDim);
        if(found != NULL){
            printf("Error : Point was already exists in K-D tree.\n");
            free(point);
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

        tree = insertPoint(tree, point, pointDim);

        free(point);

        printf("Point was successfully added to the K-D tree!\n");
        printf("KD-tree:\n");
        printTree(tree, 0);
        if(!writePointsToFile(tree, pointDim)){
            printf("Error: Cannot save points in CSV files.\n");
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }
    } 
    else if (strcmp(operation, "-kd_delete") == 0){
        double *point = NULL;
        int pointDim = 0;

        if(!parsePoint(params, &point, &pointDim)){
            printf("Error: Invalid point format.\n");
            free(point);
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }
        else if(pointDim != pointDimCSV){
            printf("Error: Point dimension does not match CSV dimension.");
            free(point);
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

        Tree *found = getPointInTree(tree, point, pointDim);
        if(found == NULL){
            printf("Error : Point was not found in K-D tree.\n");
            free(point);
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

        tree = deleteNode(tree, point, pointDim);

        printf("Point was successfully deleted from K-D tree!\n");
        printf("KD-tree:\n");
        printTree(tree, 0);

        free(point);
        if(!writePointsToFile(tree, pointDim)){
            printf("Error: Cannot save points in CSV files.\n");
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }
    }
    else if (strcmp(operation, "-kd_nearest") == 0) {
        double *point = NULL;
        int pointDim = 0;

        if(!parsePoint(params, &point, &pointDim)){
            printf("Error: Invalid point format.\n");
            free(point);
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }
        else if(pointDim != pointDimCSV){
            printf("Error: Point dimension does not match CSV dimension.");
            free(point);
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

        Tree *nearest = findNearest(tree, point, pointDim);
        if (!nearest) {
            // надо ли искать только в уже существующих? Да
            printf("Error: Nearest point was not found.\n");
            free(point);
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

        printf("Nearest Point: ");
        for (int i = 0; i < pointDim; i++) {
            printf("%lf ", nearest->point[i]);
        }
        printf("\n");
        printf("KD-tree:\n");
        printTree(tree, 0);
        free(point);
    } else if (strcmp(operation, "-dbscan") == 0) {
        double eps = 0.0;
        int minPts = 0;

        if(!parseDbscanParams(params, &eps, &minPts)){
            printf("Error: Invalid DBSCAN format\n");
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

        int clustersCount = dbscanCluster(tree, eps, minPts, countCSV);
        if(clustersCount <= 0){
            printf("Error: DBSCAN found no clusters.\n");
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

        printf("DBSCAN clusters was counted successfully!\n");

        // Сохранение результатов в файлы
        if(!writeClustersToFiles(tree, pointDimCSV, clustersCount, "dbscan")){
            printf("Error: Cannot save clusters in CSV files.\n");
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

    } else if (strcmp(operation, "-cmeans") == 0) {
        int clustersCount = 0;

        if(!parsePositiveInt(params, &clustersCount)){
            printf("Error: Invalid Fuzzy C-Means format.\n");
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }
        printf("Cmeans operation\n");
        if (!fuzzyCMeans(tree, countCSV, pointDimCSV, clustersCount)) {
            printf("Error: Fuzzy C-Means was failed.\n");
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }

        // Сделать вывод в файл
        if(!writeClustersToFiles(tree, pointDimCSV, clustersCount, "cmeans")){
            printf("Error: Cannot save clusters in CSV files.\n");
            freeKDtree(tree);
            freePointsArray(pointsCSV, countCSV);
            return 1;
        }
    }
    else{
        printf("Error: Unknown operation.\n");
        printAllFunc();
        freeKDtree(tree);
        freePointsArray(pointsCSV, countCSV);
        return 1;
    }


    // ============ FREE==============
    freeKDtree(tree);
    freePointsArray(pointsCSV, countCSV);
    return 0;
}
