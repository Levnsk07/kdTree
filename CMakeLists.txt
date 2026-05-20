cmake_minimum_required(VERSION 3.10)
project(kdTree)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -O2")

option(BUILD_TESTS "Build tests" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(kdTree SHARED kdTree.c clustering.c)
target_include_directories(kdTree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(kdTree m)

add_executable(robot_spatial main.c)
target_link_libraries(robot_spatial kdTree)

if(BUILD_TESTS)
    find_path(CMOCKA_INCLUDE_DIR cmocka.h PATHS "C:/msys64/mingw64/include")
    find_library(CMOCKA_LIBRARY cmocka PATHS "C:/msys64/mingw64/lib")
    
    if(CMOCKA_INCLUDE_DIR AND CMOCKA_LIBRARY)
        add_executable(tests tests.c)
        target_include_directories(tests PRIVATE ${CMOCKA_INCLUDE_DIR})
        target_link_libraries(tests kdTree ${CMOCKA_LIBRARY} m)
        enable_testing()
        add_test(NAME tests COMMAND tests)
    endif()
endif()

add_custom_target(run COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/robot_spatial DEPENDS robot_spatial)
add_custom_target(clean-all COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR})