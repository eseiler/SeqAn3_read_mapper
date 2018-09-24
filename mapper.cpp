#include <seqan3/io/stream/debug_stream.hpp>

int main(int argc, char const ** argv)
{
    using namespace seqan3;
    debug_stream << "Hello world\n";
    return 0;
}
