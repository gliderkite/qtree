# qtree

This repository contains a header-only C++11 implementation of a quadtree data structure.

A quadtree is a tree data structure in which each internal node has exactly four children. Quadtrees are the two-dimensional analog of octrees and are most often used to partition a two-dimensional space by recursively subdividing it into four quadrants or regions. Wikipedia: https://en.wikipedia.org/wiki/Quadtree

You can build the test executable on a Linux environment by simply running: `./setup.sh`  
Cleaning the workspace: `./setup -clean`  
Building debug version: `./setup.sh -debug` (release by default)

The test executable relies on the [googletest](https://github.com/google/googletest) framework.
