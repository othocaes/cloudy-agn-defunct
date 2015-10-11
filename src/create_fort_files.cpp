#include "agn.hpp"
#include "spectral_lines.hpp"


int main(int argc, char const *argv[]) {
	std::ifstream cloudy_result_file;
	cloudy_result_file.open(argv[1]);

	std::cout
		<< "Reading cloudy grid from "
		<< argv[1]
		<< ".\n";
	agn::cloudy_grid grid = agn::read_cloudy_grid(cloudy_result_file);

	std::cout
		<< "Reading line list from "
		<< argv[2]
		<< ".\n";
	std::ifstream line_list_file;
	line_list_file.open(argv[2]);
	agn::line_list lines_to_print = agn::read_line_list(line_list_file);
	std::cout 
		<< "Compiling table2ds for " 
		<< lines_to_print.size()
		<< " emission lines.\n";
	std::list<agn::eqwidth_table> tables = agn::compile_eqwidth_tables(grid,lines_to_print,1215.00);

	std::cout
		<< "Printing "
		<< tables.size()
		<< " tables to fortfiles.\n";
	std::list<agn::eqwidth_table>::iterator table_it = tables.begin();
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

	std::cout << "Done.\n";
	return 0;
}




