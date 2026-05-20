#include <stdarg.h> 
#include <stddef.h> 
#include <setjmp.h> 
#include <stdlib.h>
#include "C:/msys64/mingw64/include/cmocka.h"
#include "kdTree.h"

static void test_build_tree_valid(void **state){
    double pointsArray[][2] = {{1, 2}, {3, 4}};
    int count = 2, dim = 2;
    double *points[] = {pointsArray[0], pointsArray[1]};

    Tree *tree = buildKDTree(points, count, dim);
    assert_non_null(tree);

    freeKDtree(tree);
}

static void test_build_tree_invalid_args(void **state){
    double pointsArray[][2] = {{1, 2}, {3, 4}};
    int count = 2, dim = 2;
    double *points[] = {pointsArray[0], pointsArray[1]};

    Tree *tree = buildKDTree(NULL, count, 2);
    assert_null(tree);
    tree = buildKDTree(points, 0, 2);
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

    double **points = (double**)malloc(count * sizeof(double*));
    for (int i = 0; i < count; i++) {
        points[i] = pointsArray[i];
    }

    Tree *tree = buildKDTree(points, count, 2);
    assert_non_null(tree);
    for(int i=0; i < count; i++)
        assert_non_null(getPointInTree(tree, points[i], 2));
    
    free(points);
    freeKDtree(tree);
}

static void test_get_missing_point(void **state){
    double pointsArray[][2] = {{1, 2}, {3, 4}};
    int count = 2, dim = 2;
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
    assert_double_equal(1, nearest->point[0], 0.00001);

    freeKDtree(tree);
}

int main(void){
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_build_tree_valid),
        cmocka_unit_test(test_build_tree_invalid_args),
        cmocka_unit_test(test_build_tree_contains_all_points),
        cmocka_unit_test(test_get_missing_point),
        cmocka_unit_test(test_distance_point),
        cmocka_unit_test(test_tree_nearest_point),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}