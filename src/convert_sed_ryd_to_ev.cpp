
//   A program to convert a cloudy incident continuum's photon
// energy unit from Rydberg to eV. Photon energy needs to be 
// in the first column.

// ./convert_rydberg_to_eV <continuum_file>

#include "agn.hpp"

int main(int argc, char const *argv[])
{
	std::stringstream output_filename;
	output_filename << argv[1];
	output_filename << ".ineV";
	std::ofstream fileout (output_filename.str().c_str(), std::ofstream::out);

	std::ifstream filein (argv[1], std::ifstream::in);
	agn::sed_table output_table = agn::read_and_convert_sed_table(filein);

    std::cout << "Found " 
    			<< output_table.value.size()
    			<< " photon indices with "
    			;
    if (output_table.header.empty()) std::cout << "no file header.";
    else  std::cout << "a file header.";
    std::cout << "\n";

	// Write to new file
    fileout << agn::format_sed_table(output_table);

	return 0;
}

