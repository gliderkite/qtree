cmake_minimum_required(VERSION 3.5)
project(qtree CXX)

# set C++ version
set(CMAKE_CXX_STANDARD 11)

# set cmake output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set build type
if(${DEBUG})
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_BUILD_TYPE Release)
endif()

# set Google Test directories
set(GTEST_SRC_ROOT ${PROJECT_SOURCE_DIR}/${GTEST_SRC_DIR})
set(GTEST_BUILD ${PROJECT_SOURCE_DIR}/${GTEST_BUILD_DIR})
set(GMOCK_INCLUDE_DIR ${GTEST_SRC_ROOT}/googlemock/include)
set(GTEST_INCLUDE_DIR ${GTEST_SRC_ROOT}/googletest/include)
set(GMOCK_BOTH_LIBRARIES ${GTEST_BUILD}/googlemock)
set(GTEST_BOTH_LIBRARIES ${GMOCK_BOTH_LIBRARIES}/gtest)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${GMOCK_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIR})

# find gtest and gmock libraries
find_library(GMOCK_LIBRARY NAMES gmock gmockd HINTS "${GMOCK_BOTH_LIBRARIES}")
find_library(GTEST_LIBRARY NAMES gtest gtestd HINTS "${GTEST_BOTH_LIBRARIES}")
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main gtest_maind HINTS "${GTEST_BOTH_LIBRARIES}")

set(TEST_EXE_NAME qtree-test)

add_executable(${TEST_EXE_NAME}
    tests/src/RectTest.cpp
    tests/src/QuadTreeTest.cpp
)

target_link_libraries(${TEST_EXE_NAME} ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY} ${GMOCK_LIBRARY})

if(UNIX)
    target_link_libraries(${TEST_EXE_NAME} -lpthread)
endif()

