# SeqAn3 Read Mapper Tutorial

## Overview

0. [Intro](#intro)
1. [Setup](#setup)

### Intro
In this tutorial you will learn how to implement a read mapper using [SeqAn3](https://www.github.com/seqan/seqan3).<br>
Read mapping is a common task in bioinformatics and is often the first step of an in-depth analysis.
Its aim is to identify positions where a query sequence (*read*) matches with up *e* errors a *reference* sequence.
In this example we will implement a read mapper step by step.<br>
Each step will be contained within a branch in this repository, the master branch being the complete program.<br>
If you encounter problems while following this tutorial, you can create a new issue on our [github](https://www.github.com/seqan/seqan3/issues/new) or look into our [documentation](http://docs.seqan.de/seqan/3.0.0-master-user/) which also provides many examples on how to use SeqAn3.


### Setup

1. [Software](#software)
2. [Directory Structure](#directory-structure)
3. [Compiling and Running](#compiling-and-running)
4. [What happens](#setup--what-happens)

#### Software
Requirements:
  - gcc >= 7
  - cmake >= 3.2
  - git
  - Seqan3

Since SeqAn3 heavily relies on modern c++, you will need at least gcc-7 to compile the code.
Depending on your operating system there are different ways to install gcc-7 or gcc-8:

**Ubuntu >= 18.04**
```
sudo apt-get install gcc
```
**Ubuntu < 18.04**
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-7
```
**MacOS** using [Homebrew](https://brew.sh/)
```
brew install gcc@7
```
**MacOS** using [MacPorts](https://www.macports.org/)
```
sudo port install gcc-7
```

After installing `g++ --version` should print the desired version. If not, you may have to use `g++-7 --version`.

Similarly you need to install cmake and git.

#### Directory Structure
For this project, we recommend following directory layout:

```
tutorial
├── SeqAn3_read_mapper
├── build
└── seqan3
```

To set these directories up you can follow this script (note the **--recursive** when cloning SeqAn3):
```
mkdir tutorial
cd tutorial
mkdir build
git clone https://github.com/eseiler/SeqAn3_read_mapper.git
git clone --recursive https://github.com/seqan/seqan3.git
```

The directory should now look like this:
```
tutorial
├── SeqAn3_read_mapper
│   ├── CMakeLists.txt
│   ├── LICENSE
│   ├── README.md
│   ├── mapper.cpp
│   ├── query.fastq
│   ├── query_small.fastq
│   ├── reference.fasta
│   └── reference_small.fasta
├── build
└── seqan3
    ├── LICENSE
    ├── README.md
    ├── build_system
    ├── include
    ├── submodules
    └── test
```

#### Compiling and Running

To test whether everything works, we will now compile and run a small example.

```bash
cd SeqAn3_read_mapper
git checkout 01_setup_app
cd ../build
cmake ../SeqAn3_read_mapper -DCMAKE_CXX_COMPILER=g++-7 -DSEQAN3_DIR=../seqan3
make
./mapper
```

The output should be `Hello world`.

<h4 id='setup--what-happens'>What happens</h4>

We use cmake as out build software.
Calling
```bash
cmake ../SeqAn3_read_mapper -DCMAKE_CXX_COMPILER=g++-7 -DSEQAN3_DIR=../seqan3
```
makes cmake look for a `CMakeLists.txt` inside `../SeqAn3_read_mapper`.<br>
`-DCMAKE_CXX_COMPILER=g++-7` tells cmake which compiler to use for our project (in this example `g++-7`) and `-DSEQAN3_DIR=../seqan3` is used within our `CMakeLists.txt`.

Let's have a closer look at the `CMakeLists.txt`:
```cmake
cmake_minimum_required (VERSION 3.2)
project (seqan3_workshop CXX)

find_package (SeqAn3 REQUIRED
              HINTS ${SEQAN3_DIR}/build_system)

add_executable (mapper
                mapper.cpp)

target_link_libraries (mapper seqan3::seqan3)
```
First we set a minimum required cmake version and give our c+\+\-project a name.<br>
Next, we want to detect SeqAn3 and tell cmake to look into `${SEQAN3_DIR}/build_system` to look for a file to use for setting up SeqAn3.<br>
Now we can add source files to our project and name the executable `mapper`.<br>
Last but not least, we can link SeqAn3 with `target_link_libraries (mapper seqan3::seqan3)`.

Inside our `mapper.cpp` we find:
```cpp
#include <seqan3/io/stream/debug_stream.hpp>

int main(int argc, char const ** argv)
{
    using namespace seqan3;
    debug_stream << "Hello world\n";
    return 0;
}
```
The first line includes our [debug_stream](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1debug__stream__type.html#details), which enables us to easily stream data to stderr - even types that normally can't be simply streamed like the [std::vector](https://de.cppreference.com/w/cpp/container/vector).<br>
To avoid having to prefix every object of our library that we want to use with `seqan3::`, we set our namespace to `seqan3`. This allows us to write `debug_stream` instead of `seqan3::debug_stream`.<br>
Next, we print `Hello world` followed by a new line and exit the program with an exit code of 0.
