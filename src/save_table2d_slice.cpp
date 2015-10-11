// Prints a 2d table of an emission flux read from a
// file (arg1), as a function of  phi for a constant
// hden (arg 2). Table is printed to a file (arg 3).

#include "spectral_lines.hpp"

int main(int argc, char const *argv[])
{
	std::ifstream filein(argv[1]);
	agn::eqwidth_table input_table = agn::read_eqwidth_table(filein);
	filein.close();
	std::ofstream fileout(argv[3]);
	fileout << agn::format_table1d(input_table.value[atof(argv[2])]);
	return 0;
}