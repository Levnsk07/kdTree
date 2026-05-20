if [[ ! -d "./build" ]]; then
  mkdir "build"
fi
gcc -shared -fPIC kdTree.c clustering.c -o ./build/kdTree.so
cd build
gcc ../main.c -o ./robot_spatial -I. -L. ./kdTree.so -lm
# ./build/robot_spatial -dbscan 0.5,5