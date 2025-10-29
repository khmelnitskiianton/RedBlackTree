# RedBlackTree
Red-black tree implementation on C++

[![MIPT](https://img.shields.io/endpoint?style=plastic&url=https%3A%2F%2Fraw.githubusercontent.com%2Fkhmelnitskiianton%2FRedBlackTree%2Fmain%2F.github%2Fbadge%2Fmipt-badge.json)](#)

[![License](https://img.shields.io/github/license/khmelnitskiianton/RedBlackTree)](#)
[![GitHub Actions](https://img.shields.io/badge/GitHub_Actions-2088FF?logo=github-actions&logoColor=white)](#)

[![C++](https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white)](#)
[![CMake](https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=fff)](#)
[![Markdown](https://img.shields.io/badge/Markdown-%23000000.svg?logo=markdown&logoColor=white)](#)

## Dependencies

Compiler c/c++ and libstdc++ >= 13, cmake, python, gtest, boost, graphviz

```shell
apt-get install build-essential ninja-build make cmake python3 libboost libboost-filesystem-dev graphviz
apt-get gcc-13 g++-13 libstdc++-13-dev
apt-get install libgtest-dev libgmock-dev libtbb-dev
```

## Building

Using CMake, you can specify 2 options:
- `SANITIZE`: enables sanitizers for gcc/clang compilers.
- `BUILD_TESTS`: build unit tests with GTest.

### Config

*Debug:*
```shell
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_TESTS=ON -DSANITIZE=ON -DCMAKE_CXX_COMPILER=g++-13 -S . -B build
```

*Release:*
```shell
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-13 -DBUILD_TESTS=ON -S . -B build
```

### Build
```shell
cmake --build build
```

Binaries are located in `build/bin/`

### Install 

```shell
cmake --install build --prefix ~/my_software
```

## Usage

Functional:
+ `insert(key)` insert key in balanced tree
+ `rangeQuery(first, second)` method returns amount of number in `[first, second)`

Input:
```
k 10 k 20 q 8 31 q 6 9 k 30 k 40 q 15 40
```
Output:
```
2 0 2 
```

## Visualisation

Using GraphViz tool, you can print red-black tree to HTML

Enable logging tree in constructor with second argument as true(default false)

```cpp
Tree::RBTree<int, bool (*)(int, int)> rbtree(comparator, true);
```

To print current state of tree use `printTree()` method.

Storage of logs: `/tmp/rbtree_log/log.html`, all objects has format SVG.

Example:
![logs](.github/images/image.png)

## Tests

Google Tests, for run do:

```shell
ctest --test-dir build/tests/unittests --output-on-failure
```

Python Tests, for run do:

```shell
python3 tests/e2e/run.py --bin build/bin/rbtree
```

## Benchmarks

To confirm optimality algorithm of my red-black tree and range query method, I create big tests and compare work time of program based on my RBTree and std::set (with its method that used lower_bound and std::distance)

Time measured by `time` utility.

Compare results with byte-equivalence:
```shell
cmp -s output_set.txt output_rbtree.txt && echo "IDENTICAL" || echo "DIFFERENT"
```

Test 1 (1000000 keys, 100000 requests):
```shell
./build/bin/set < tests/bench/big_test_000001.txt > output_set.txt  394,97s user 1,44s system 98% cpu 6:40,96 total
./build/bin/rbtree < tests/bench/big_test_000001.txt > output_rbtree1.txt  5,20s user 0,36s system 80% cpu 6,924 total
```

Test 2 (10000000 keys, 100 requests):
```shell
./build/bin/set < tests/bench/big_test_000002.txt > output_set.txt  28,35s user 0,28s system 99% cpu 28,629 total
./build/bin/rbtree < tests/bench/big_test_000002.txt > output_rbtree.txt  30,94s user 0,40s system 99% cpu 31,344 total
```

Test 3 (10000 keys, 10000000 requests):
```shell
./build/bin/set < tests/bench/big_test_000003.txt > output_set.txt  89,96s user 65,19s system 40% cpu 6:24,32 total
./build/bin/rbtree < tests/bench/big_test_000003.txt > output_rbtree.txt  31,56s user 58,62s system 32% cpu 4:36,04 total
```

Test 4 (1000000 keys, 1000000 requests):
```shell
# set run for 30 min and didnt end
./build/bin/rbtree < tests/bench/big_test_000004.txt > output_rbtree.txt  8,22s user 5,01s system 39% cpu 33,478 total
```

Analysis:
+ `insert` method is 1.1 times faster in the 'std::set' structure.
+ `range_query` method is 3 times faster in this red-black tree implementation

Script for generating tests: `tests/bench/gen.py`

## Workflow

Pipeline runs in 2 stages:
- `build`: debug build project & create artifact.
- `tests`: run tests for build
    + `UnitTests`: tests/unittests run with GTest
    + `End2EndTests`: tests/e2e run with Python