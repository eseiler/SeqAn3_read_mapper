#include <seqan3/alignment/configuration/all.hpp>
#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/io/alignment_file/output.hpp>
#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/search/fm_index/fm_index.hpp>
#include <seqan3/search/algorithm/all.hpp>

#include <range/v3/action/slice.hpp>

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

    std::cout << "Indexing reference sequence.\n";
    // Index reference genome
    fm_index<std::vector<dna5>> index{genome};

    auto search_cfg = search_cfg::max_error(search_cfg::total{max_error},
                                            search_cfg::substitution{max_error},
                                            search_cfg::insertion{max_error},
                                            search_cfg::deletion{max_error})
                                            | search_cfg::mode(search_cfg::all_best);

    alignment_file_output sam_file{sam_file_path};

    // add the genome information to the reference genome dictionary of the header
    sam_file.header().ref_dict[genome_id] = {genome.size(), std::string{}};

    unsigned i = 0;
    for (auto & [id, query] : query_file)
    {
        auto positions = search(index, query, search_cfg);
        debug_stream << "id:\t\t" << id << '\n';
        debug_stream << "query:\t\t" << query << '\n';
        for (size_t position : positions)
        {
            auto database_view = genome | ranges::view::slice(position, position + query.size() + max_error);
            debug_stream << "position:\t" << position << '\n';
            debug_stream << "database:\t" << database_view << '\n';

            auto align_sequences = std::make_pair(database_view, query);
            auto align_cfg = align_cfg::edit | align_cfg::sequence_ends<free_ends_at::seq1>
                                             | align_cfg::output<align_result_key::trace>;

            for (auto && alignment : align_pairwise(align_sequences, align_cfg))
            {
                using aligned_sequence_type = std::pair<std::vector<gapped<dna5>>, std::vector<gapped<dna5>>>;
                aligned_sequence_type aligned_sequence = alignment.trace();
                debug_stream << "score:\t\t" << alignment.score() << '\n';
                debug_stream << "gapped_database:" << std::get<0>(aligned_sequence) << '\n';
                debug_stream << "gapped_query:\t"  << std::get<1>(aligned_sequence) << '\n';

                using sam_types = type_list<std::vector<dna5>, std::string, std::string, aligned_sequence_type>;
                using sam_type_ids = fields<field::SEQ, field::ID, field::REF_ID, field::ALIGNMENT>;
                using sam_record_type = record<sam_types, sam_type_ids>;

                sam_record_type record{query, id, genome_id, aligned_sequence};
                sam_file.push_back(record);
            }
        }

        debug_stream << "======================" << '\n';
        if (++i >= 20)
            break;
    }

    return 0;
}
