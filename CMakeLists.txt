cmake_minimum_required (VERSION 3.2)
project (seqan3_workshop CXX)

# Find SeqAn3
find_package (SeqAn3 REQUIRED
              HINTS ${SEQAN3_DIR}/build_system)

# Update the list of file names below if you add source files to your project.
add_executable (mapper
                mapper.cpp)

# Link the SeqAn3 library
target_link_libraries (mapper seqan3::seqan3)
