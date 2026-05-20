#include <stdarg.h> 
#include <stddef.h> 
#include <setjmp.h> 
#include <stdlib.h>
#include "C:/msys64/mingw64/include/cmocka.h"
#include "kdTree.h"

static void test_build_tree_null(void **state){
    int count=0;
    int dim=2;
    double **points = NULL;
    
    Tree *tree = buildKDTree(points, count, dim);
    assert_null(tree);

    tree = buildKDTree(points, 0, dim);
    assert_null(tree);

    tree = buildKDTree(points, count, 0);
    assert_null(tree);

    freeKDtree(tree);
}

static void test_build_tree_valid(void **state){
    double pointsArray[][2] = {{1, 2}, {3, 4}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[] = {pointsArray[0], pointsArray[1]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    freeKDtree(tree);
}

static void test_build_tree_invalid_args(void **state){
    double pointsArray[][2] = {{1, 2}, {3, 4}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[] = {pointsArray[0], pointsArray[1]};

    Tree *tree = buildKDTree(NULL, count, dim);
    assert_null(tree);
    tree = buildKDTree(points, 0, dim);
    assert_null(tree);
    tree = buildKDTree(points, count, 0);
    assert_null(tree);

    freeKDtree(tree);
}

static void test_build_tree_contains_all_points(void **state){
    double pointsArray[][2] = {{1, 1}, {1, 2}, {2, 1},
                                {8, 8}, {8, 9}, {9, 8},
                                {20, 20}, {21, 20}, {20, 21}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;

    double **points = (double**)malloc(count * sizeof(double*));
    for (int i = 0; i < count; i++) {
        points[i] = pointsArray[i];
    }

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);
    for(int i=0; i < count; i++)
        assert_non_null(getPointInTree(tree, points[i], dim));
    
    free(points);
    freeKDtree(tree);
}

static void test_build_large_tree(void **state){
    int count = 1000;
    int dim = 2;
    double **points = (double**)malloc(count * sizeof(double*));
    for(int i=0; i < count; i++){
        points[i] = (double*)malloc(dim*sizeof(double));
        points[i][0] = rand() % 1000;
        points[i][1] = rand() % 1000;
    }

    Tree *tree = buildKDTree(points, count, dim);

    for(int i=0; i < count; i++)
        assert_non_null(getPointInTree(tree, points[i], dim));

    double point[] = {500, 500};
    Tree *nearest = findNearest(tree, point, dim);
    assert_non_null(nearest);

    for(int i=0; i < count; i++)
        free(points[i]);
    free(points);
    freeKDtree(tree);
}

static void test_get_point_null_tree(void **state){
    double point[] = {1, 1};
    int dim = 2;
    Tree *result = getPointInTree(NULL, point, dim);
    assert_null(result);
}

static void test_get_missing_point(void **state){
    double pointsArray[][2] = {{1, 2}, {3, 4}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[] = {pointsArray[0], pointsArray[1]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    double point[] = {2, 3};
    assert_null(getPointInTree(tree, point, dim));

    freeKDtree(tree);
}

static void test_distance_point(void **state){
    double a[] = {0, 0};
    double b[] = {3, 4};
    int dim = 2;

    assert_double_equal(distancePoint(a, b, dim), 5.0, 0.000001);
}

static void test_insert_point(void **state){
    double pointsArray[][2] = {{5, 5}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[] = {pointsArray[0]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    double newPoint[] = {3, 7};
    tree = insertPoint(tree, newPoint, dim);

    assert_non_null(getPointInTree(tree, newPoint, dim));
    assert_non_null(getPointInTree(tree, points[0], dim));

    freeKDtree(tree);
}

static void test_insert_duplicate_point(void **state){
    double pointsArray[][2] = {{1, 2}, {3, 4}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[] = {pointsArray[0], pointsArray[1]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    double newPoint[] = {3, 4};
    tree = insertPoint(tree, newPoint, dim);

    assert_non_null(getPointInTree(tree, points[1], dim));

    tree = deleteNode(tree, points[1], dim);

    assert_null(getPointInTree(tree, points[1], dim));

    freeKDtree(tree);
}

static void test_delete_leaf_node(void **state){
    double pointsArray[][2] = {{5, 5}, {3, 3}, {7, 7}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[] = {pointsArray[0], pointsArray[1], pointsArray[2]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    tree = deleteNode(tree, pointsArray[2], dim);
    assert_null(getPointInTree(tree, pointsArray[2], dim));
    assert_non_null(getPointInTree(tree, pointsArray[0], dim));
    assert_non_null(getPointInTree(tree, pointsArray[1], dim));

    freeKDtree(tree);
}

static void test_delete_root_node(void **state){
    double pointsArray[][2] = {{5, 5}, {3, 3}, {7, 7}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[] = {pointsArray[0], pointsArray[1], pointsArray[2]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    tree = deleteNode(tree, pointsArray[0], dim);
    assert_null(getPointInTree(tree, pointsArray[0], dim));
    assert_non_null(getPointInTree(tree, pointsArray[1], dim));
    assert_non_null(getPointInTree(tree, pointsArray[2], dim));

    freeKDtree(tree);
}

static void test_delete_nonexistent_point(void **state){
    double pointsArray[][2] = {{5, 5}, {3, 3}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[] = {pointsArray[0], pointsArray[1]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    double nonesixtent[] = {1, 1};

    Tree *result = deleteNode(tree, nonesixtent, dim);
    assert_non_null(result);
    assert_non_null(getPointInTree(tree, pointsArray[0], dim));
    assert_non_null(getPointInTree(tree, pointsArray[1], dim));

    freeKDtree(tree);
}

static void test_find_nearest_null_tree(void **state){
    double point[] = {1, 1};
    int dim = 2;
    Tree *nearest = findNearest(NULL, point, dim);
    assert_null(nearest);
}

static void test_tree_nearest_point(void **state){
    double pointsArray[][2] = {{1, 1}, {5, 5}, {10, 10}};
    int count = 3, dim = 2;

    double *points[] = {pointsArray[0], pointsArray[1], pointsArray[2]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    double point[] = {1.2, 1.1};

    Tree *nearest = findNearest(tree, point, dim);
    assert_non_null(nearest);
    assert_double_equal(1, nearest->point[0], 0.00001);
    assert_double_equal(1, nearest->point[1], 0.00001);

    freeKDtree(tree);
}

static void test_single_point_nearest(void **state){
    double pointsArray[][2] = {{1, 1}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;

    double *points[] = {pointsArray[0]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    Tree *nearest = findNearest(tree, points[0], dim);
    assert_null(nearest);

    freeKDtree(tree);
}

static void test_dbscan_basic(void **state){
    double pointsArray[][2] = {
        {1, 1}, {1, 2}, {2, 1},
        {8, 8}, {8, 9}, {9, 8}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[6];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    int clusters = dbscanCluster(tree, 2.0, 2, count);
    assert_int_equal(clusters, 2);

    int clusterIdx1 = getPointInTree(tree, points[0], dim)->clusterIdx;
    for(int i=1; i < 3; i++)
        assert_int_equal(getPointInTree(tree, points[i], dim)->clusterIdx, clusterIdx1);

    int clusterIdx2 = getPointInTree(tree, points[3], dim)->clusterIdx;
    for(int i=4; i < count; i++)
        assert_int_equal(getPointInTree(tree, points[i], dim)->clusterIdx, clusterIdx2);

    assert_int_not_equal(clusterIdx1, clusterIdx2);
    freeKDtree(tree);
}

static void test_dbscan_with_noise(void **state){
    double pointsArray[][2] = {
        {1, 1}, {1, 2}, {2, 1},
        {100, 100}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[4];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    int clusters = dbscanCluster(tree, 2.0, 2, count);
    assert_int_equal(clusters, 1);

    Tree *noisePoint = getPointInTree(tree, points[3], dim);
    assert_int_equal(noisePoint->clusterIdx, -1);

    for(int i=0; i < 3; i++)
        assert_int_not_equal(getPointInTree(tree, points[i], dim)->clusterIdx, -1);
    
    freeKDtree(tree);
}

static void test_dbscan_single_cluster(void **state){
    double pointsArray[][2] = {
        {1, 1}, {1, 2}, {2, 1},
        {2, 2}, {1.5, 1.5}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[5];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    int clusters = dbscanCluster(tree, 1.5, 2, count);

    assert_int_equal(clusters, 1);

    int clusterIdx1 = getPointInTree(tree, points[0], dim)->clusterIdx;
    for(int i=1; i < count; i++)
        assert_int_equal(getPointInTree(tree, points[i], dim)->clusterIdx, clusterIdx1);

    freeKDtree(tree);
}

static void test_dbscan_invalid_args(void **state){
    double pointsArray[][2] = {
        {1, 1}, {1, 2}, {2, 1},
        {8, 8}, {8, 9}, {9, 8}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[6];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    int result = dbscanCluster(NULL, 1.0, 2, count);
    assert_int_equal(result, 0);

    result = dbscanCluster(tree, -1.0, 2, count);
    assert_int_equal(result, 0);

    result = dbscanCluster(tree, 1.0, 0, count);
    assert_int_equal(result, 0);

    result = dbscanCluster(tree, 1.0, 2, 0);
    assert_int_equal(result, 0);

    freeKDtree(tree);
}

static void test_fuzzy_cmeans_basic(void **state){
    double pointsArray[][2] = {
        {1, 1}, {1, 2}, {2, 1},
        {8, 8}, {8, 9}, {9, 8}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[6];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    bool result = fuzzyCMeans(tree, count, dim, 2);
    assert_true(result);

    Tree *node = NULL;
    for(int i=0; i < count; i++){
        node = getPointInTree(tree, points[i], dim);
        assert_int_not_equal(node->clusterIdx, -1);
        assert_non_null(node->memberShip);
    }
    int clusterIdx1 = getPointInTree(tree, points[0], dim)->clusterIdx;
    for(int i=1; i < 3; i++)
        assert_int_equal(getPointInTree(tree, points[i], dim)->clusterIdx, clusterIdx1);

    int clusterIdx2 = getPointInTree(tree, points[3], dim)->clusterIdx;
    for(int i=4; i < count; i++)
        assert_int_equal(getPointInTree(tree, points[i], dim)->clusterIdx, clusterIdx2);

    assert_int_not_equal(clusterIdx1, clusterIdx2);
    freeKDtree(tree);
}

static void test_fuzzy_cmeans_single_cluster(void **state){
    double pointsArray[][2] = {
        {1, 1}, {1, 2}, {2, 1},
        {2, 2}, {1.5, 1.5}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[5];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    int clustersCount = 1;

    bool result = fuzzyCMeans(tree, count, dim, clustersCount);
    assert_true(result);

    Tree *node = NULL;
    for(int i=0; i < count; i++){
        node = getPointInTree(tree, points[i], dim);
        assert_int_equal(node->clusterIdx, 0);
        assert_non_null(node->memberShip);
    }

    freeKDtree(tree);
}

static void test_fuzzy_cmeans_convergence(void **state){
    double pointsArray[][2] = {
        {1, 1}, {1, 2}, {2, 1},
        {8, 8}, {8, 9}, {9, 8}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[6];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    int clustersCount = 2;

    bool result = fuzzyCMeans(tree, count, dim, clustersCount);
    assert_true(result);

    Tree *node = NULL;
    double sum = 0.0;
    for(int i=0; i < count; i++){
        node = getPointInTree(tree, points[i], dim);
        sum = 0.0;
        for(int j=0; j < clustersCount; j++)
            sum += node->memberShip[j];
        assert_double_equal(sum, 1.0, 0.00001);
    }

    freeKDtree(tree);
}

static void test_fuzzy_cmeans_invalid_args(void **state){
    double pointsArray[][2] = {
        {1, 1}, {1, 2}, {2, 1},
        {8, 8}, {8, 9}, {9, 8}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 2;
    double *points[6];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    bool result = fuzzyCMeans(NULL, count, dim, 2);
    assert_false(result);

    result = fuzzyCMeans(tree, -1, dim, 3);
    assert_false(result);

    result = fuzzyCMeans(tree, count, -1, 3);
    assert_false(result);

    result = fuzzyCMeans(tree, count, dim, -1);
    assert_false(result);

    result = fuzzyCMeans(tree, count, dim, count+1);
    assert_false(result);

    freeKDtree(tree);
}

static void test_3d_build_tree(void **state){
    double pointsArray[][3] = {
        {1, 2, 3}, {4, 5, 6}, {7, 8, 9},
        {10, 11, 12}, {13, 14, 15}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 3;
    double *points[5];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    for(int i=0; i < count; i++)
        assert_non_null(getPointInTree(tree, points[i], dim));
    
    freeKDtree(tree);
}

static void test_3d_insert_point(void **state){
    double pointsArray[][3] = {{1, 2, 3}};
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 3;
    double *points[1];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    double newPoint[] = {4, 5, 6};
    tree = insertPoint(tree, newPoint, dim);

    assert_non_null(getPointInTree(tree, newPoint, dim));
    assert_non_null(getPointInTree(tree, points[0], dim));

    freeKDtree(tree);
}

static void test_3d_delete_point(void **state){
    double pointsArray[][3] = {
        {5, 5, 5}, {3, 3, 3}, {7, 7, 7}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 3;
    double *points[3];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);
    
    tree = deleteNode(tree, points[2], dim);
    assert_null(getPointInTree(tree, points[2], dim));
    assert_non_null(getPointInTree(tree, points[0], dim));
    assert_non_null(getPointInTree(tree, points[1], dim));

    freeKDtree(tree);
}

static void test_3d_nearest_point(void **state){
    double pointsArray[][3] = {
        {1, 1, 1}, {5, 5, 5}, {10, 10, 10}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 3;
    double *points[3];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    double point[] = {1.1, 1.2, 1.3};
    Tree *nearest = findNearest(tree, point, dim);
    assert_non_null(nearest);
    assert_double_equal(1, nearest->point[0], 0.00001);
    assert_double_equal(1, nearest->point[1], 0.00001);
    assert_double_equal(1, nearest->point[2], 0.00001);

    freeKDtree(tree);
}

static void test_3d_distance(void **state){
    double a[] = {0, 0, 0};
    double b[] = {3, 4, 12};
    int dim = 3;

    assert_double_equal(distancePoint(a, b, dim), 13.0, 0.00001);
}

static void test_3d_dbscan_clustering(void **state){
    double pointsArray[][3] = {
        {1, 1, 1}, {1, 2, 1}, {2, 1, 1},
        {8, 8, 8}, {8, 9, 8}, {9, 8, 8},
        {100, 100, 100}
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 3;
    double *points[7];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    int clusters = dbscanCluster(tree, 2.0, 2, count);
    
    assert_int_equal(clusters, 2);

    int clusterIdx1 = getPointInTree(tree, points[0], dim)->clusterIdx;
    for(int i=1; i < 3; i++)
        assert_int_equal(getPointInTree(tree, points[i], dim)->clusterIdx, clusterIdx1);

    int clusterIdx2 = getPointInTree(tree, points[3], dim)->clusterIdx;
    for(int i=4; i < 6; i++)
        assert_int_equal(getPointInTree(tree, points[i], dim)->clusterIdx, clusterIdx2);

    assert_int_equal(getPointInTree(tree, points[6], dim)->clusterIdx, -1);

    assert_int_not_equal(clusterIdx1, clusterIdx2);

    freeKDtree(tree);
}

static void test_3d_fuzzy_cmeans(void **state){
    double pointsArray[][3] = {
        {1, 1, 1}, {1, 2, 1}, {2, 1, 1},
        {8, 8, 8}, {8, 9, 8}, {9, 8, 8},
    };
    int count = sizeof(pointsArray) / sizeof(pointsArray[0]);
    int dim = 3;
    double *points[6];
    for(int i=0; i < count; i++)
        points[i] = pointsArray[i];

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    bool result = fuzzyCMeans(tree, count, dim, 2);
    assert_true(result);

    Tree *node = NULL;
    for(int i=0; i < count; i++){
        node = getPointInTree(tree, points[i], dim);
        assert_int_not_equal(node->clusterIdx, -1);
        assert_non_null(node->memberShip);
    }
    
    int clusterIdx1 = getPointInTree(tree, points[0], dim)->clusterIdx;
    for(int i=1; i < 3; i++)
        assert_int_equal(getPointInTree(tree, points[i], dim)->clusterIdx, clusterIdx1);

    int clusterIdx2 = getPointInTree(tree, points[3], dim)->clusterIdx;
    for(int i=4; i < 6; i++)
        assert_int_equal(getPointInTree(tree, points[i], dim)->clusterIdx, clusterIdx2);

    assert_int_not_equal(clusterIdx1, clusterIdx2);

    freeKDtree(tree);
}

int main(void){
    srand(0);
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_build_tree_null),
        cmocka_unit_test(test_build_tree_valid),
        cmocka_unit_test(test_build_tree_invalid_args),
        cmocka_unit_test(test_build_tree_contains_all_points),
        cmocka_unit_test(test_build_large_tree),
        cmocka_unit_test(test_get_point_null_tree),
        cmocka_unit_test(test_get_missing_point),
        cmocka_unit_test(test_distance_point),
        cmocka_unit_test(test_insert_point),
        cmocka_unit_test(test_insert_duplicate_point),
        cmocka_unit_test(test_delete_leaf_node),
        cmocka_unit_test(test_delete_root_node),
        cmocka_unit_test(test_delete_nonexistent_point),
        cmocka_unit_test(test_find_nearest_null_tree),
        cmocka_unit_test(test_tree_nearest_point),
        cmocka_unit_test(test_single_point_nearest),
        cmocka_unit_test(test_dbscan_basic),
        cmocka_unit_test(test_dbscan_with_noise),
        cmocka_unit_test(test_dbscan_single_cluster),
        cmocka_unit_test(test_dbscan_invalid_args),
        cmocka_unit_test(test_fuzzy_cmeans_basic),
        cmocka_unit_test(test_fuzzy_cmeans_single_cluster),
        cmocka_unit_test(test_fuzzy_cmeans_convergence),
        cmocka_unit_test(test_fuzzy_cmeans_invalid_args),

        cmocka_unit_test(test_3d_build_tree),
        cmocka_unit_test(test_3d_insert_point),
        cmocka_unit_test(test_3d_delete_point),
        cmocka_unit_test(test_3d_nearest_point),
        cmocka_unit_test(test_3d_distance),
        cmocka_unit_test(test_3d_dbscan_clustering),
        cmocka_unit_test(test_3d_fuzzy_cmeans)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}