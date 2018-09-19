#include <seqan3/alignment/configuration/all.hpp>
#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/range/view/to_char.hpp>
#include <seqan3/range/view/persist.hpp>
#include <seqan3/search/fm_index/fm_index.hpp>
#include <seqan3/search/fm_index/fm_index_iterator.hpp>
#include <seqan3/search/algorithm/all.hpp>
#include <seqan3/alignment/configuration/all.hpp>
#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/alphabet/gap/gapped.hpp>

#include <range/v3/action/slice.hpp>

int main(int argc, char const ** argv)
{
    using namespace seqan3;
    using namespace seqan3::literal;
<<<<<<< HEAD
    using namespace seqan3::search_cfg;
    using namespace seqan3::align_cfg;
||||||| merged common ancestors
    using namespace seqan3::search_cfg;
=======
>>>>>>> alignment works

    // Initialise the argument parser. The program name will be mapper, and we pass the arguments.
    argument_parser myparser("Mapper", argc, argv);

    // This will hold the path to the reference sequence
    std::string reference;
    // This will hold the path to the query sequence
    std::string query;
    // This will hold the maximum allowed number of errors for which we want to map and set the default to 0
    int max_error_no{0};

    // We add a positional option to pass the reference sequence to our program
    // Positional options are always required
    // We set "Path to reference" as a description of the option
    myparser.add_positional_option(reference, "Path to reference");
    // We add another option to pass the query sequence to our program
    // We set "Path to reference" as a description of the option
    myparser.add_positional_option(query, "Path to query");
    // We add another option to specify the maximum allowed errors
    // We will be able to use either -e <number> or --error <number>
    // We set "Maximum allowed errors" as a description of the option
    // This option is not required and the default value 0 will be used if the option is not provided
    myparser.add_option(max_error_no, 'e', "error", "Maximum allowed errors");

    // We parse the arguments
    try
    {
        myparser.parse();
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

    // constexpr bool debug{true};
    std::cout << "Loading reference file.\n";

    // Read reference sequence
    // The format of the input file will be automatically determined
    // Since we have a FASTA file, we will be able to access the ID of sequences (lines that start with '>')
    // and the actual sequence (SEQ)
    sequence_file_input ref{reference};
    // We store our reference sequence in `genome`
    // Our genome will be a vector (sequence) over the dna5 (A,C,G,T,N) alphabet
    // We can access the sequence of our reference by requesting the sequence (get<field::SEQ>) of the first entry
    // in the reference file (*ref.begin())
    std::vector<dna5> genome = get<field::SEQ>(*ref.begin());

    // if constexpr(debug)
    // {
    //     std::cout << genome.size() << '\n';
    //     auto g = genome | view::take(50);
    //     debug_stream << g;
    //     debug_stream << '\n';
    // }
    std::cout << "Indexing reference sequence.\n";
    // Index reference genome
    fm_index<std::vector<dna5>> index{genome};

    sequence_file_input queries{query, fields<field::ID, field::SEQ>{}};

    auto search_cfg = search_cfg::max_error(search_cfg::total{max_error_no},
                                            search_cfg::substitution{max_error_no},
                                            search_cfg::insertion{max_error_no},
                                            search_cfg::deletion{max_error_no})
                                            | search_cfg::mode(search_cfg::all_best);

    for (auto & [id, query] : queries)
    {
        auto positions = search(index, query, search_cfg);
        debug_stream << "id:\t\t" << id << '\n';
        debug_stream << "query:\t\t" << query << '\n';
        for (size_t position : positions)
        {
            auto database_view = genome | ranges::view::slice(position, position + query.size() + max_error_no);
            debug_stream << "position:\t\t" << position << '\n';
            debug_stream << "database:\t" << database_view << '\n';

            auto align_sequences = std::make_pair(database_view, query);
            auto align_cfg = align_cfg::edit | align_cfg::output<align_result_key::trace>;

            for (auto && alignment : align_pairwise(align_sequences, align_cfg))
            {
                auto && [gapped_database, gapped_query] = alignment.trace();
                // TODO: change this when https://github.com/seqan/seqan3/issues/458 is fixed
                // debug_stream << "database:\t" << gapped_database << '\n';
                debug_stream << "database:\t" << (gapped_database | view::to_char) << '\n';
                debug_stream << "query:\t\t" << (gapped_query | view::to_char) << '\n';
            }
        }
        break;
    }
}
