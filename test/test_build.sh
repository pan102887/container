CURRENT_PATH=$(dirname "$0")
CMAKE_BUILD_DIR=$CURRENT_PATH/cmake_build
if [ ! -e $CMAKE_BUILD_DIR ];then
    mkdir $CMAKE_BUILD_DIR
fi
cd $CMAKE_BUILD_DIR
cmake ..
make