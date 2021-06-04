# FastMysticSquare
[![FastMysticSquare Build](https://github.com/BiagioFesta/FastMysticSquare/actions/workflows/Build.yml/badge.svg)](https://github.com/BiagioFesta/FastMysticSquare/actions/workflows/Build.yml)
[![codecov](https://codecov.io/gh/BiagioFesta/FastMysticSquare/branch/master/graph/badge.svg?token=XDVH0L9HPE)](https://codecov.io/gh/BiagioFesta/FastMysticSquare)
[![cppstd](https://img.shields.io/badge/C++-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)

## Introduction
*FastMysticSquare* is a very *fast* solver of the [15 Puzzle Problem](https://en.wikipedia.org/wiki/15_puzzle), written in *C++*.

![](https://raw.githubusercontent.com/BiagioFesta/FastMysticSquare/master/img/screenshot.png)

---

## Compilation
### Requirements
 * Compiler with C++17 support;
 * [CMake](https://cmake.org/) as *build generator* (`version >= 3.14`).
 
### CMake Options:
   * `CMAKE_BUILD_TYPE` specifies the type: `Release` or `Debug` (default);
   * `BUILD_TESTS` specifies whether you want to build *unit tests*: `OFF` (default) or `ON`;
   * `CMAKE_INSTALL_PREFIX` the installation prefix (default: `/usr/local` on UNIX and `C:/Program Files` on Windows).

### Instructions
 
* Clone the repository and open the project directory:
```
git clone https://github.com/BiagioFesta/FastMysticSquare.git
```
```
cd FastMysticSquare
```

*  Create a build directory:
```
mkdir build
```
```
cd build
```

* Generate compilation files:
```
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DCMAKE_INSTALL_PREFIX=./opt ..
```

* Run compilation:
```
cmake --build . --config Release
```

* Install executable:
```
cmake --install . --config Release
```

## Usage

 ~~~
 Usage:
  ./opt/bin/FastMysticSquare [OPTION...]

  -h, --help                    Display this help message.
  -a, --algorithm {MANHATTAN|PATTERN}
                                Select the heuristic algorithm to use.
  -s, --state {RANDOM|0,1,2,3,...}
                                Select the initial state of the problem.
  -i, --interactive             Enables the interactive mode.
 ~~~
