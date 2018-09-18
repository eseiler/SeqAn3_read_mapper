#include <seqan3/argument_parser/all.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/io/stream/debug_stream.hpp>
// #include <seqan3/range/view/take.hpp>
// #include <seqan3/range/view/to_char.hpp>
#include <seqan3/range/view/persist.hpp>
#include <seqan3/search/fm_index/fm_index.hpp>
#include <seqan3/search/fm_index/fm_index_iterator.hpp>
#include <seqan3/search/algorithm/all.hpp>

#include <range/v3/action/slice.hpp>

int main(int argc, char const ** argv)
{
    using namespace seqan3;
    using namespace seqan3::literal;
    using namespace seqan3::search_cfg;

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

    detail::configuration const cfg = max_error(total{max_error_no},
                                                substitution{max_error_no},
                                                insertion{max_error_no},
                                                deletion{max_error_no})
                                                | mode(best);

    for (auto & [id, seq] : queries)
    {
        auto results = search(index, seq, cfg);
        for (auto res : results)
        {
            auto m = genome | ranges::view::slice(res, res + seq.size() + max_error_no);
            debug_stream << m << '\n';
        }

    }

/*
    // debug_stream << results << '\n';
    // align_pairwise(std::make_tuple(genome | view::slice(3, 28), seq), align_cfg::edit)
    // auto it = index.begin();
    // if (it.extend_right(seq))
    // {
    //     debug_stream << "Read ID: " << id << " Number of hits: " << it.count() << '\n';
    //     for (auto const & pos : it.locate())
    //     {
    //         debug_stream << pos << ' ';
    //     }
    //     debug_stream << '\n';
    // }
    // For each read
    sequence_file_in reads{"reads.fastq"};

    for (auto & record : reads)
    {
        // search
        // ...
        // align
        // ...
    }

    // align
    // ...
*/
}
