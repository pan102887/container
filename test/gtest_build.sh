CURRENT_PATH=$(dirname "$0")
GTEST_BUILD_DIR=$CURRENT_PATH/../lib/googletest/build
if [ ! -e $GTEST_BUILD_DIR ]; then
    mkdir $GTEST_BUILD_DIR
fi
cd $GTEST_BUILD_DIR
cmake ..
make
