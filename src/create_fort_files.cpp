#include "agn.hpp"
#include "spectral_lines.hpp"


int main(int argc, char const *argv[]) {
	std::ifstream cloudy_result_file;
	cloudy_result_file.open(argv[1]);

    std::cout
        << "Reading line list from "
        << argv[2]
        << ".\n";
    std::ifstream line_list_file;
    line_list_file.open(argv[2]);
    agn::line_list lines_to_print = agn::read_line_list(line_list_file);

	std::cout
		<< "Reading cloudy grid from "
		<< argv[1]
		<< ".\n";
	agn::cloudy_grid grid = agn::read_cloudy_grid(cloudy_result_file, lines_to_print);

	std::cout
		<< "Compiling table2ds for "
		<< lines_to_print.size()
		<< " emission lines.\n";
	std::list<agn::eqwidth_table> tables = 
        agn::compile_eqwidth_tables(grid,lines_to_print,1215.00);


    // Remove any tables that are zero.
    std::list<agn::eqwidth_table>::iterator table_it = tables.begin();
    std::ofstream zeroreport;
    zeroreport.open("zero_report");
    zeroreport << "These headers were pulled from tables"
                << " that returned minimum values."
                << std::endl;
    int num_zeroes=0;
    while(table_it != tables.end()) {
        if(agn::is_zero(*table_it)) {
            zeroreport << " "
                        << table_it->header[0]
                        << std::endl;
            num_zeroes++;
            table_it = tables.erase(table_it);
            continue;
        }

    table_it++;
    }
    std::cout << "Removed "
            << num_zeroes
            << " tables from the list because"
            << " they had zero value."
            << std::endl;

    // Write the tables to the fortfile stack.
	std::cout
		<< "Printing "
		<< tables.size()
		<< " tables to fortfiles.\n";
    table_it = tables.begin();
	int fortfilenum=11;
	while(table_it != tables.end()) {
		std::ofstream outfile;
		std::stringstream filename;
		filename << "fort.";
		filename << fortfilenum;
		outfile.open(filename.str().c_str());
		outfile << *table_it;
		outfile.close();
		table_it++;
		fortfilenum++;
	}

    int num_unconverged = 0;
    agn::cloudy_grid::iterator result_it = grid.begin();
    std::ofstream cautionreportfile;
    cautionreportfile.open("cautions");
    cautionreportfile
        << "The following solutions probably did not converge."
        << std::endl << std::endl;
    while(result_it != grid.end()) {
        if (result_it->second.iterations >= 40) {
            num_unconverged++;
            cautionreportfile
                << "hden = "
                << std::fixed
                << std::setprecision(3)
                << result_it->second.hden
                << ", phi = "
                << result_it->second.phi
                << std::endl
                << "───────────────────────────"
                << std::endl;
            std::list<std::string>::iterator caution_it =
                result_it->second.cautions.begin();
            while(caution_it != result_it->second.cautions.end()) {
                cautionreportfile
                    << *caution_it
                    << std::endl;
                    caution_it++;
            }
            cautionreportfile << std::endl << std::endl;
        }
        result_it++;
    }

    std::cout
        << "Saved cautions for "
        << num_unconverged
        << " unconverged solutions."
        << std::endl;
        cautionreportfile.close();

	std::cout << "Done.\n";
	return 0;
}

