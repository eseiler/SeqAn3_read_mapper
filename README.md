# This tutorial is now part of the [Documentation](http://docs.seqan.de/seqan/3.0.0-master-user/tutorial_read_mapper.html)

---
---

# SeqAn3 Read Mapper Tutorial

## Overview

0. [Intro](#intro)
1. [Setup](#setup)
2. [Argument Parser](#argument-parser)
3. [Sequence File Input](#sequence-file-input)
4. [Indexing](#indexing)
5. [Search](#search)
6. [Alignment](#alignment)
7. [Alignment File Output](#alignment-file-output)

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
In some situations it can happen that the correct assembler is not found. 
You will see an error during the cmake configuration that says something like: `... could not understand flag m ...`.
In this case you can try to export the Path:
```
export PATH=/util/bin:$PATH
```
and try running cmake again.

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

### Argument Parser
1. [Usage](#argparse--usage)
2. [What happens](#argparse--what-happens)

<h4 id='argparse--usage'>Usage</h4>

As a first step, we want to be able to pass parameters to our program via the command line.
To make life easier, we provide a general purpose [argument parser](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1argument__parser.html#details) in SeqAn3.<br>
To follow this step, you need to checkout the `02_argument_parser` branch of this repository, e.g. by using `git checkout 02_argument_parser` while being in the `SeqAn3_read_mapper` directory.<br>
Since our source code changed by switching branches, we need to recompile our project with

```bash
make mapper
```
while being in the `build` directory. Note that we do not need to run cmake again since we did not change any settings regarding how the project is built (e.g. adding a source file to the `CMakeLists.txt`, switching the compiler or compiling with different options).<br>
Now we can pass parameters to the executable:
```bash
./mapper ../SeqAn3_read_mapper/reference_small.fasta ../SeqAn3_read_mapper/query_small.fastq -e 2 -o my.sam
```
Which will print:
```bash
reference_file_path:	../SeqAn3_read_mapper/reference_small.fasta
query_file_path:	../SeqAn3_read_mapper/query_small.fastq
max_error:		2
sam_file_path:		my.sam
```
The argument parser also checks whether the arguments are valid, e.g. passing a character as max error:
```bash
./mapper ../SeqAn3_read_mapper/reference_small.fasta ../SeqAn3_read_mapper/query_small.fastq -e f -o my.sam
```
will throw an error:
```
[PARSER ERROR] Value cast failed for option -e: Argument f could not be casted to type INT (32 bit)
```
Passing no arguments will result in a short description on how to use the mapper:
```bash
./mapper
```
Output:
```bash
Mapper
======
    Mapper mapper [-e max_error] [-o sam_output_file] <reference file path> <query file path>
    Try -h or --help for more information.
```
And also help messages are automatically generated:
```bash
./mapper -h
```
Will print:
```bash
Mapper
======

SYNOPSIS
    Mapper mapper [-e max_error] [-o sam_output_file] <reference file path> <query file path>

POSITIONAL ARGUMENTS
    ARGUMENT-1 STRING
          Path to reference genome
    ARGUMENT-2 STRING
          Path to query

OPTIONS
    -e, --error INT (32 bit)
          Maximum allowed errors
    -o, --output STRING
          Path to output sam file

VERSION
    Last update:
    Mapper version:
    SeqAn version: 3.0.0
```

<h4 id='argparse--what-happens'>What happens</h4>

Our `mapper.cpp` now also uses `#include <seqan3/argument_parser/all.hpp>` in order to use our argument parser.

```cpp
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/io/stream/debug_stream.hpp>

int main(int argc, char const ** argv)
{
    using namespace seqan3;

    argument_parser parser("Mapper", argc, argv);
    parser.info.synopsis.push_back("mapper [-e max_error] [-o sam_output_file] <reference file path> <query file path>");
```
We first instantiate an `argument_parser` called `parser`. The first argument is our program name (*mapper*), and we pass the argument count (*argc*) and argument vector (*argv*).<br>
Next, we add a short usage description that will be printed when using `-h, --help` or no arguments at all.<br>
We plan to add four arguments: the reference file, query file, maximum error number and the output file.
To do this, we first define 4 variables that will hold the respective value:
```cpp
    std::string reference_file_path;
    std::string query_file_path;
    std::string sam_file_path;
    int max_error{0};
```
We do not set any default values for the files, but we define the default error number as `0`.<br>
Now we can tell the argument parser which arguments we want to parse:
```cpp
    parser.add_positional_option(reference_file_path, "Path to reference genome");
    parser.add_positional_option(query_file_path, "Path to query");
    parser.add_option(max_error, 'e', "error", "Maximum allowed errors");
    parser.add_option(sam_file_path, 'o', "output", "Path to output sam file");
```
We add the reference and query file as a positional argument, i.e. we do not need to pass an identifier such as `-r` or `--reference`, but instead expect the reference file to be passed as first argument without identifier and the query as second argument. `add_positional_option` takes to arguments: first the variable where the result from parsing should be stored and second a description that will be shown in the help message.<br>
We can also add options that require an identifier; in our example the `max_error` and `sam_file_path`. Here, we need two additional arguments: a short identifier (`e` and `o`) and a long identifer (`error` and `output`). The short identifier can be used with a single dash (e.g. `-e`) whereas the long identifier requires two dashes to be parser correctly (e.g. `--error`). <br>
To learn more about the capabilities of the argument parser, e.g. [checking for valid file extensions](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1file__ext__validator.html#details) or using [flags](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1argument__parser.html#details), and more examples, you can have a look at the [documentation](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1argument__parser.html#details).<br>
Once we configured our argument parser, we need to actually parse the arguments provided to the program using the `parse()` method. Since the passed arguments are checked for validity, we always want to surrond `parse()` with a `try/catch` block to treat errors that may occurr during parsing:
```cpp
    try
    {
        parser.parse();
    }
    catch (seqan3::parser_invalid_argument const & ext)
    {
        std::cerr << "[PARSER ERROR] " << ext.what();
        return -1;
    }
    catch (seqan3::parser_interruption const & ext)
    {
        return 0;
    }
```
`parse()` considers many scenarios where something goes wrong with the parsing and throws an appropiate [exception](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1argument__parser.html#a74b8e529f79c6056427260c488d52ca2) in these cases.<br>
In general, we want to catch `seqan3::parser_invalid_argument`, which is thrown - as the name suggests - when a passed argument does not fulfil the requirements we defined. Whenever an error is thrown, we can also customize the error message. <br>
Another "exception" is the `seqan3::parser_interruption` which represents an expected event such as printing the help message. In this case we want to return a 0 as exit status since it is expected behaviour.<br>
After calling `parse()` we are done with the argument parsing!<br>
To verify that it works as expected, we print the parsed arguments to our [debug_stream](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1debug__stream__type.html#details):
```cpp
    debug_stream << "reference_file_path:\t" << reference_file_path << '\n';
    debug_stream << "query_file_path:\t"     << query_file_path << '\n';
    debug_stream << "max_error:\t\t"         << max_error << '\n';
    debug_stream << "sam_file_path:\t\t"     << sam_file_path << '\n';
```
and exit with a status of 0.
```cpp
    return 0;
}
```

### Sequence File Input
1. [Usage](#seqfilein--usage)
2. [What happens](#seqfilein--what-happens)

<h4 id='seqfilein--usage'>Usage</h4>

In this part, we want to read our reference and query files.
To try out this part, you need to checkout the `03_sequence_file_input` branch and rebuild the executable.
SeqAn3 provides the [sequence_file_input](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1sequence__file__input.html#details) class which will help us with reading out input files.
Running
```bash
./mapper ../SeqAn3_read_mapper/reference_small.fasta ../SeqAn3_read_mapper/query_small.fastq -e f -o my.sam
```
will now also print the first 100 nucleotides of our reference as well as the first 20 read identifier and sequences (output shortened):
```
reference_file_path:	../SeqAn3_read_mapper/reference_small.fasta
query_file_path:	../SeqAn3_read_mapper/query_small.fastq
max_error:		2
sam_file_path:		my.sam
Loading reference file.
genome: 1 -> TATGCACCAGAGTATGGAAGCATAAGCTCTGCATGCAAAGGTACATCAGATCCTGCGGTTGGGTGCCAACCCAAGTGTGTTCACGGGCGCTTGACAGACA
id:		simulated.1
query:		GCCTGTGACGGGATCCTGCTTAATGCCCGGTACGCAAATGCACCGAGCGGAAAGATCTGTGTCTAGGTTAATCTATGTATTTACTCATGACTAGGTGAAT
[...]
id:		simulated.20
query:		GGTCTCTTCGTTATCAACGTCGTGGATGTCGGCGGCCGCTTTCCTGCTAAGACTTAATCCTTTCTCGACGCGTGGCAATCCTTGGGAGTGTGTCCTGCAT
```

<h4 id='seqfilein--what-happens'>What happens</h4>

First, we need to include the correct header to use the [sequence_file_input](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1sequence__file__input.html#details):
```cpp
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/io/stream/debug_stream.hpp>

int main(int argc, char const ** argv)
{
```
After parsing, we can now open the reference file:
```
    sequence_file_input reference_file{reference_file_path};
```
Each entry in the sequence file (e.g. ID+Sequence) is treated as a record, over which we can iterate exactly once (single pass input).
Since we know that our reference contains exactly one record, we can directly access the first iterator with `begin()`.
Additionally, we do not want to store the actual record, but rather the underlying sequence and identifier. For this, we can use `get` to extract the information from the record.
```cpp
    std::vector<dna5> genome = get<field::SEQ>(*reference_file.begin());
    std::string genome_id = get<field::ID>(*reference_file.begin());
```
To print the first 100 neucleotides we can use `view::take`. You can think of views as objects that apply a function to something and can be concatenated by "piping". Views do not change the data on which they are applied and don't take ownership of data, i.e. they are light-weight and cheap to copy (like iterators). Another key property is that they are lazy evaluated, that means that only when you access the view, the operations are actually carried out. To learn more about views and the closely related ranges, you can have a look at [TODO](link). In our example, we use `view::take`:
```cpp
    debug_stream << "genome: " << genome_id << " -> " << (genome | view::take(100)) << '\n';
```
Note the parenthesis surrounding `genome | view::take(100)`. This is necessary because the `<<` operator has a higher precedence than `|` and would be evaluated before the view is applied otherwise.<br>
Now we want to read our queries:
```cpp
    sequence_file_input query_file{query_file_path, fields<field::ID, field::SEQ>{}};
```
By adding `fields<field::ID, field::SEQ>{}` we can tell the `sequence_file_input` to just consider the `ID` and the `SEQ`. This handy when iterating over records, since we can directly decompose the returned values:
```cpp
    unsigned i = 0;
    for (auto & [id, query] : query_file)
    {
        debug_stream << "id:\t\t" << id << '\n';
        debug_stream << "query:\t\t" << query << '\n';

        if (++i >= 20)
            break;
    }
```

### Indexing
1. [Usage](#index--usage)
2. [What happens](#index--what-happens)

<h4 id='index--usage'>Usage</h4>

In order to allow an efficient search, we first index our reference sequence using an [FM-Index](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1fm__index.html#details).
This step is part of the `04_fm_index` branch and rebuilding + running the program will produce almost the same output as in the last step - the only difference being a `Indexing reference sequence.` output added.

<h4 id='index--what-happens'>What happens</h4>

We first include the corresponding header:
```cpp
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/search/fm_index/fm_index.hpp>

int main(int argc, char const ** argv)
{
```
And then create a default FM-Index on the reference:
```cpp
    fm_index<std::vector<dna5>> index{genome};
```
Where `std::vector<dna5>` is the same type as the type of `genome`.<br>
Internally we use the FM-Index implementation of the [Succinct Data Structure Library (SDSL) Version 3](https://github.com/xxsds/sdsl-lite), more information can be found at the [Version 2 SDSL Repository](https://github.com/simongog/sdsl-lite).<br>
The FM-Index can be highly customised to accomodate various needs, if you would like more information on this, you can have a look at our [documentation](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1fm__index.html#details).

### Search
1. [Usage](#search--usage)
2. [What happens](#search--what-happens)

<h4 id='search--usage'>Usage</h4>

Now that we have an index, we can search for all occurrences of a read in the reference with up to *e* errors.<br>
To try out the search for yourself, you need to checkout the `05_search` path and rebuild the program. Running
```
./mapper ../SeqAn3_read_mapper/reference_small.fasta ../SeqAn3_read_mapper/query_small.fastq -e 2 -o my.sam
```
Will print again print the first 20 reads, but if there is a match to the reference with at most 2 errors, the position and the region of the reference where the all best hits occur will be printed:
```
reference_file_path:	../SeqAn3_read_mapper/reference_small.fasta
query_file_path:	../SeqAn3_read_mapper/query_small.fastq
max_error:		2
sam_file_path:		my.sam
Loading reference file.
genome: 1 -> TATGCACCAGAGTATGGAAGCATAAGCTCTGCATGCAAAGGTACATCAGATCCTGCGGTTGGGTGCCAACCCAAGTGTGTTCACGGGCGCTTGACAGACA
Indexing reference sequence.
id:		simulated.1
query:		GCCTGTGACGGGATCCTGCTTAATGCCCGGTACGCAAATGCACCGAGCGGAAAGATCTGTGTCTAGGTTAATCTATGTATTTACTCATGACTAGGTGAAT
======================
id:		simulated.2
query:		GGATGGTCTATTCACTGACGCAGAGTAACAGAATCTATTTATGGGCCGCCGTGGTAGAAACTTAGTAGTGTCCCTCATCCCGATAATATCCCGATGAGAA
======================
id:		simulated.3
query:		AAGGCAACCCACCTCGGGGCGATTGTGGTCACTGCTTAGAATGCTCATGCTATTTCTGGTAATCCGCCGGAGTAAGGGAGGCAGTGACAACTCGATTCGC
======================
id:		simulated.4
query:		GTCTGTGTCAGTGGATCCCTGGACCAGATTGCAGTTCAAAAAAGAGCTAAATGGTTACTGATGTGCGTCAGACGCCACCCGGTAGCCCCGGGTTCGAGGA
position:	55026
database:	GTCTGTGTCAGTGGATCCCTGGACCAGATTGCAGTTCAAAAAAGAGCTAAATGGTTACTGATGTGCGTCAGACGCCACCCGGTAGCCCCGGGTTCGAGGAAA
[...]
id:		simulated.20
query:		GGTCTCTTCGTTATCAACGTCGTGGATGTCGGCGGCCGCTTTCCTGCTAAGACTTAATCCTTTCTCGACGCGTGGCAATCCTTGGGAGTGTGTCCTGCAT
======================
```

<h4 id='search--what-happens'>What happens</h4>

First, we include `<seqan3/search/algorithm/all.hpp>` for the search and `<range/v3/action/slice.hpp>` to easily access the region where a hit occurred.<br>
After indexing our reference, we need to create a [search configuration](http://docs.seqan.de/seqan/3.0.0-master-user/namespaceseqan3_1_1search__cfg.html#details) object which describes how to conduct the search:
```cpp
    auto search_cfg = search_cfg::max_error(search_cfg::total{max_error},
                                        search_cfg::substitution{max_error},
                                        search_cfg::insertion{max_error},
                                        search_cfg::deletion{max_error})
                                        | search_cfg::mode(search_cfg::all_best);
```
We first create a `search_cfg::max_error` object which contains a `search_cfg::total` for the overall allowed errors as well as a configuration for each error type; in this example we are looking for matches with a maximum edit distance of 2. Using the `|` operator we concatenate the error config with a search mode (best, all_best or all). In this example we the search to report all hits that have the lowest edit distance.<br>
Now we can conduct a search for each query and store the reported positions:
```cpp
        auto positions = search(index, query, search_cfg);
```
For each position we can now extract the region from the reference where the hit occurs. Since we do not know if there were any insertion, we need to extract a region of size `querylength + max_error` in case there were two insertions.
```cpp
        for (size_t position : positions)
        {
            auto database_view = genome | ranges::view::slice(position, position + query.size() + max_error);
            debug_stream << "position:\t" << position << '\n';
            debug_stream << "database:\t" << database_view << '\n';
        }
```

### Alignment
1. [Usage](#align--usage)
2. [What happens](#align--what-happens)

<h4 id='align--usage'>Usage</h4>

Now that we have a query sequence and the corresponding region if the reference where a hit occurrs, we can construct a [pairwise alignment](http://docs.seqan.de/seqan/3.0.0-master-user/align__pairwise_8hpp.html#details) to generate an alignment.
Checking out `06_align_pairwise` and running the program again will yield:
```
reference_file_path:	../SeqAn3_read_mapper/reference_small.fasta
query_file_path:	../SeqAn3_read_mapper/query_small.fastq
max_error:		2
sam_file_path:		my.sam
Loading reference file.
genome: 1 -> TATGCACCAGAGTATGGAAGCATAAGCTCTGCATGCAAAGGTACATCAGATCCTGCGGTTGGGTGCCAACCCAAGTGTGTTCACGGGCGCTTGACAGACA
Indexing reference sequence.
id:		simulated.1
query:		GCCTGTGACGGGATCCTGCTTAATGCCCGGTACGCAAATGCACCGAGCGGAAAGATCTGTGTCTAGGTTAATCTATGTATTTACTCATGACTAGGTGAAT
======================
id:		simulated.2
query:		GGATGGTCTATTCACTGACGCAGAGTAACAGAATCTATTTATGGGCCGCCGTGGTAGAAACTTAGTAGTGTCCCTCATCCCGATAATATCCCGATGAGAA
======================
id:		simulated.3
query:		AAGGCAACCCACCTCGGGGCGATTGTGGTCACTGCTTAGAATGCTCATGCTATTTCTGGTAATCCGCCGGAGTAAGGGAGGCAGTGACAACTCGATTCGC
======================
id:		simulated.4
query:		GTCTGTGTCAGTGGATCCCTGGACCAGATTGCAGTTCAAAAAAGAGCTAAATGGTTACTGATGTGCGTCAGACGCCACCCGGTAGCCCCGGGTTCGAGGA
position:	55026
database:	GTCTGTGTCAGTGGATCCCTGGACCAGATTGCAGTTCAAAAAAGAGCTAAATGGTTACTGATGTGCGTCAGACGCCACCCGGTAGCCCCGGGTTCGAGGAAA
score:		0
gapped_database:GTCTGTGTCAGTGGATCCCTGGACCAGATTGCAGTTCAAAAAAGAGCTAAATGGTTACTGATGTGCGTCAGACGCCACCCGGTAGCCCCGGGTTCGAGGA
gapped_query:	GTCTGTGTCAGTGGATCCCTGGACCAGATTGCAGTTCAAAAAAGAGCTAAATGGTTACTGATGTGCGTCAGACGCCACCCGGTAGCCCCGGGTTCGAGGA
[...]
id:		simulated.20
query:		GGTCTCTTCGTTATCAACGTCGTGGATGTCGGCGGCCGCTTTCCTGCTAAGACTTAATCCTTTCTCGACGCGTGGCAATCCTTGGGAGTGTGTCCTGCAT
======================
```

<h4 id='align--what-happens'>What happens</h4>

First, we add includes to allow the usage of the pairwise alignment:
```cpp
#include <seqan3/alignment/configuration/all.hpp>
#include <seqan3/alignment/pairwise/align_pairwise.hpp>
```
Similarly to the search, we need to specify what kind of alignment we want by creating a configuration:
```cpp
            auto align_cfg = align_cfg::edit | align_cfg::sequence_ends<free_ends_at::seq1>
                                             | align_cfg::output<align_result_key::trace>;
```
We want to use the edit distance as metric, allow free ends in the first sequence (the reference) and also want to be able to get the alignment trace as an output.
The `align_pairwise` function expects a pair as input, therefore we a pair of query and reference region after we find a hit:
```cpp
    auto align_sequences = std::make_pair(database_view, query);
```
The pairwise alignment may report multiple possible alignments that achieve the same score, i.e. we have to iterate over the results:
```cpp
            for (auto && alignment : align_pairwise(align_sequences, align_cfg))
            {
```
We also need to define what type the returned aligned sequences should have. In out case we expect a pair of dna5 sequences that may contain gaps:
```cpp
                using aligned_sequence_type = std::pair<std::vector<gapped<dna5>>, std::vector<gapped<dna5>>>;
```
Now we can request the trace of the alignment and print the corresponding score and sequences:
```cpp
                aligned_sequence_type aligned_sequence = alignment.trace();
                debug_stream << "score:\t\t" << alignment.score() << '\n';
                debug_stream << "gapped_database:" << std::get<0>(aligned_sequence) << '\n';
                debug_stream << "gapped_query:\t"  << std::get<1>(aligned_sequence) << '\n';
            }
```

### Alignment File Output
1. [Usage](#alignout--usage)
2. [What happens](#alignout--what-happens)

<h4 id='alignout--usage'>Usage</h4>

Last but not least, we want to [store our alignments](http://docs.seqan.de/seqan/3.0.0-master-user/classseqan3_1_1alignment__file__output.html#details) in the SAM format.
This step happens in the `07_alignment_file_output`. It will produce no different console output compared to the step before but will create the SAM file specified by the `-o` parameter:
```
@HD	VN:1.6	SO:unknown	GO:none
@SQ	SN:1	LN:100000
simulated.4	0	1	0	0	100M	*	0	0	GTCTGTGTCAGTGGATCCCTGGACCAGATTGCAGTTCAAAAAAGAGCTAAATGGTTACTGATGTGCGTCAGACGCCACCCGGTAGCCCCGGGTTCGAGGA	*
simulated.6	0	1	0	0	66M1I33M	*	0	0	TGCGGGCCCCATCGCGAACCCTACACGTGTAATGTACATTTCCCGCCCGTTAACCGTTCCGTGGGCCTAGTCTTGCTAGTCATTGTGGACGCGTCTATAC	*
simulated.7	0	1	0	0	100M	*	0	0	ACCTGCCCTATTACGGAGGACTTGGTCATCGCACCAGGCCCGTCTTGGAATGGGGGATCGCGGGGAGCCTACTTAATGGCTCGTAAACGCGCGTAGCGGT	*
simulated.10	0	1	0	0	100M	*	0	0	CTTTCTGTGCTTGCATCCAGTCTCTGCTGTCATCTTAAGACGATCCATAATAATGGCACGCGAGGTTGGATGGTGGGTTCGATGTCCTCCTGCCGATCTG	*
simulated.11	0	1	0	0	100M	*	0	0	CAGGTAGCGATGTCGTTTACACAGTGTATACCTTCAGGGATACATAGACTATTGATTTTCGCCGTGTACCACTGCCGCGCCGGATAAGGCGATGAAGGTT	*
simulated.12	0	1	0	0	100M	*	0	0	GGTCGTTCCGCACGCGGGAGAGCGACTGTTCTGGCCCGCCGAGCCTTCACACTCTTGTTAGTCTGGCTCACCTAGGCCGGAAGTAGATGTCCAGTCAAGA	*
simulated.13	0	1	0	0	100M	*	0	0	GTTATGGGCTATATAACGGGAAGTTTATCGTCCTGACTGGCGCACATAATATGCTCCGGATCAAGCCCTAATTCGGTCAAACGACTATTTTCGCCATGGT	*
simulated.14	0	1	0	0	100M	*	0	0	CGCGATTTTAAGTTGGATCAGATCCAGCCACTTGTGATGGTTTGGACGCCATGGCGCGAAGTCTCGGAAGAGGCGGTGCAGAAAGTGCTCGTTTAGTCAA	*
simulated.15	0	1	0	0	100M	*	0	0	GTCACAGTCACGGGCTCCTTTATCATATACATAGGTGACCTTGCTAAGGGCAACCTCTACAGATACCAGCATGAGCTGGATACGCGTAGAGGCTGGCAGA	*
simulated.16	0	1	0	0	100M	*	0	0	TCTTTTGGTGTTGCAAATTCGAGCTAATCCACTCCAAGCGGCGACCAAGCTGAGTTCCGTGCAAGACCGCCGGCGCTTTTTAGGTTCCCCCGAGAGGGCA	*
simulated.17	0	1	0	0	100M	*	0	0	TTTCTCCTTATTTATGATACACGTTCCACTACTGCTATGTGAGAAGCGGCCTAGCCGTGGCCCAGCGGTCGGGCCGTGCCTTATATGAAGGGCGTTCCCC	*
simulated.18	0	1	0	0	100M	*	0	0	TCGTACTTTAGATAGCTGGCGCAGTAAGTATTACGTGCACGCGACAATAGACCAATAATCGAAGTACAATATATGTATGGTGTCGTACTCCCTTTGGAAA	*
```

<h4 id='alignout--what-happens'>What happens</h4>

First, we add `#include <seqan3/io/alignment_file/output.hpp>` to use the alignment_file_output.
The we tell it to store out alignment in the file that is given via the `-o` argument:
```cpp
    alignment_file_output sam_file{sam_file_path};
```
Nest, we need to specify what the header of the SAM file should contain. We need to add the reference genome to the header:
```cpp
    sam_file.header().ref_dict[genome_id] = {genome.size(), std::string{}};
```
Now, whenever we retrieve a valid alignment and want to add a record to the SAM file, we need to specify which of the SAM fields we want to set and of which type they are:
```
                using sam_types = type_list<std::vector<dna5>, std::string, std::string, aligned_sequence_type>;
                using sam_type_ids = fields<field::SEQ, field::ID, field::REF_ID, field::ALIGNMENT>;
                using sam_record_type = record<sam_types, sam_type_ids>;
```
After this, we can add a record that describes the alignment between query and reference:
```cpp
                sam_record_type record{query, id, genome_id, aligned_sequence};
                sam_file.push_back(record);
```
