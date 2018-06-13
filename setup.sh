#!/bin/bash

set -e

BUILD_DIR="build"
PROJECT_BUILD_DIR="${BUILD_DIR}/qtree"
GTEST_REPOSITORY="https://github.com/google/googletest.git"
GTEST_SRC_DIR="googletest"
GTEST_BUILD_DIR="${BUILD_DIR}/gtest"

CLEAN=false
DEBUG=false

# parse arguments
for key in $@
do
case $key in
    -clean)
    CLEAN=true
    ;;
    -debug)
    DEBUG=true
    ;;
esac
# past argument
shift
done

# clean the environment
if [ ${CLEAN} = true ]; then
    rm -rf ${BUILD_DIR}
    exit 0
fi

# update gtest submodule
git submodule init
git submodule update

if [ ! -d ${GTEST_BUILD_DIR} ]; then
    mkdir -p ${GTEST_BUILD_DIR}
    (cd ${GTEST_BUILD_DIR}
    cmake ../../${GTEST_SRC_DIR} && make)
fi

CMAKE_ARGS=(
    -DGTEST_SRC_DIR=${GTEST_SRC_DIR}
    -DGTEST_BUILD_DIR=${GTEST_BUILD_DIR}
    -DDEBUG=${DEBUG}
)

mkdir -p ${PROJECT_BUILD_DIR}
(cd ${PROJECT_BUILD_DIR}
cmake ../.. ${CMAKE_ARGS[@]} && make)
