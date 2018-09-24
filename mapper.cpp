#include <seqan3/argument_parser/all.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/io/stream/debug_stream.hpp>

int main(int argc, char const ** argv)
{
    using namespace seqan3;

    // Initialise the argument parser. The program name will be mapper, and we pass the arguments.
    argument_parser parser("Mapper", argc, argv);
    parser.info.synopsis.push_back("mapper [-e max_error] [-o sam_output_file] <reference file path> <query file path>");

    // This will hold the path to the reference sequence
    std::string reference_file_path;
    // This will hold the path to the query sequence
    std::string query_file_path;
    // This will hold the path to the sam output file
    std::string sam_file_path;
    // This will hold the maximum allowed number of errors for which we want to map and set the default to 0
    int max_error{0};

    // We add a positional option to pass the reference sequence to our program
    // Positional options are always required
    // We set "Path to reference" as a description of the option
    parser.add_positional_option(reference_file_path, "Path to reference genome");
    // We add another option to pass the query sequence to our program
    // We set "Path to reference" as a description of the option
    parser.add_positional_option(query_file_path, "Path to query");
    // We add another option to specify the maximum allowed errors
    // We will be able to use either -e <number> or --error <number>
    // We set "Maximum allowed errors" as a description of the option
    // This option is not required and the default value 0 will be used if the option is not provided
    parser.add_option(max_error, 'e', "error", "Maximum allowed errors");
    // Path to output sam file
    parser.add_option(sam_file_path, 'o', "output", "Path to output sam file");

    // We parse the arguments
    try
    {
        parser.parse();
    }
    catch (seqan3::parser_invalid_argument const & ext) // the user did something wrong
    {
        std::cerr << "[PARSER ERROR] " << ext.what(); // customize your error message
        return -1;
    }
    catch (seqan3::parser_interruption const & ext) // expected behaviour on special requests (e.g. `--help`)
    {
        return 0;
    }

    debug_stream << "reference_file_path:\t" << reference_file_path << '\n';
    debug_stream << "query_file_path:\t"     << query_file_path << '\n';
    debug_stream << "max_error:\t\t"         << max_error << '\n';
    debug_stream << "sam_file_path:\t\t"     << sam_file_path << '\n';

    std::cout << "Loading reference file.\n";

    // Read reference sequence
    // The format of the input file will be automatically determined
    // Since we have a FASTA file, we will be able to access the ID of sequences (lines that start with '>')
    // and the actual sequence (SEQ)
    sequence_file_input reference_file{reference_file_path};
    // We store our reference sequence in `genome`
    // Our genome will be a vector (sequence) over the dna5 (A,C,G,T,N) alphabet
    // We can access the sequence of our reference by requesting the sequence (get<field::SEQ>) of the first entry
    // in the reference file (*ref.begin())
    std::vector<dna5> genome = get<field::SEQ>(*reference_file.begin());
    std::string genome_id = get<field::ID>(*reference_file.begin());
    debug_stream << "genome: " << genome_id << " -> " << (genome | view::take(100)) << '\n';

    sequence_file_input query_file{query_file_path, fields<field::ID, field::SEQ>{}};

    unsigned i = 0;
    for (auto & [id, query] : query_file)
    {
        debug_stream << "id:\t\t" << id << '\n';
        debug_stream << "query:\t\t" << query << '\n';

        if (++i >= 20)
            break;
    }

    return 0;
}
