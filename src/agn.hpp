#ifndef agn_hpp
#define agn_hpp

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <cctype>
#include <cstdlib>
#include <set>
#include <cmath>

namespace agn {

const bool debug = false;

// General constants
const double PI=3.14159265358979323846;
const double PLANCK_CONST=4.135668e-15; // in eV * s
const double BOLTZMANN_CONST=0.00008617332385; // in eV / K
const double RYDBERG_CONST=1.0973731568539e7; // in 1 / m
const double RYDBERG_UNIT_EV=13.60569252; // in eV
const double RYDBERG_UNIT_ANGSTROM=1e10/RYDBERG_CONST; // in A

// Some useful containers and functions.
typedef std::map<double,double> table1d;
typedef table1d::iterator iterator1d;
std::string format_table1d(table1d);

typedef std::map<double,table1d> table2d;
typedef table2d::iterator iterator2d;
typedef std::pair<double,double> coord2d;
typedef std::list<coord2d> gridcoordlist;

struct cloudy_line_data {
	double radiated_energy,eq_width;
	int index;
	bool has_duplicates;
};

typedef std::map<std::string,cloudy_line_data> cloudy_line_output;

struct cloudy_result {	
	std::string header, footer;
	std::list<std::string> emergent_line_raw_text;
	std::list<std::string> cautions;
	cloudy_line_output emergent_line_intensity;
	int iterations;
	double phi,hden,colden;
	cloudy_result():
		header(""),
		footer(""),
		emergent_line_raw_text(),
		cautions(),
		iterations(0),
		phi(0),
		hden(0),
		colden(0) 
		{}
};

typedef std::map<coord2d,cloudy_result> cloudy_grid;
//	Easiest to read the entire grid from the file at once.
cloudy_grid read_cloudy_grid(std::ifstream&);
//	Operator<< prints general info about the run result.
std::ostream& operator<< (std::ostream&, cloudy_result);
//  Seeks an instream to the line after the first occurrence of seek_string.
void seek_to(std::string,std::istream&); 


} // end namespace agn
































// Definitions






void agn::seek_to(std::string seek_string,std::istream& stream) {
	std::string test_str="";
	while (test_str.find(seek_string) == std::string::npos)
		getline(stream,test_str);
}



std::string agn::format_table1d(agn::table1d table) {
    std::stringstream output;
    output 
        << std::scientific
        << std::setprecision(5);
    agn::table1d::iterator x=table.begin();
    while(x!=table.end()) {
        output
            << x->first
            << "\t"
            << x->second
            << "\n";
        x++;
    }
    return output.str();
}

std::ostream& agn::operator<< (std::ostream& outstream, cloudy_result output) {
	outstream.width(5);
	outstream
		<< std::fixed
		<< std::setprecision(2)
		<< output.hden
		<< " x "
		<< output.phi
		<< " (colden "
		<< std::fixed
		<< std::setprecision(0)
		<< output.colden
		<< "): "
		<< output.emergent_line_intensity.size()
		<< " emission lines, "
		<< output.cautions.size()
		<< " cautions present.";
	return outstream;
}

std::ifstream& operator>> (std::ifstream& inputfile,agn::cloudy_grid& grid) {
	grid = agn::read_cloudy_grid(inputfile);
	return inputfile;
}




agn::cloudy_grid agn::read_cloudy_grid(std::ifstream& inputfile) {
	if(agn::debug) std::cout
		<< "Constructing cloudy output grid from file.\n";
	inputfile.clear();
	inputfile.seekg(0);
	std::string inputline;
	int line_num=0;
	getline(inputfile,inputline);
	std::string curstr = inputline.substr(0,23);
	std::string seek_string = "Producing grid output.";
	int pos = curstr.find(seek_string);
	while(line_num < 1000) {
		line_num++;
		getline(inputfile,inputline);
		curstr = inputline.substr(0,30);
		pos = inputline.find(seek_string);
		if (pos != std::string::npos) break;
	}
	if(agn::debug) std::cout 
		<< "Collecting grid meta data starting on line " 
		<< line_num 
		<< std::endl;
	agn::gridcoordlist coordlist;
	std::pair<double,double> xy (0,0);
	seek_string="**************************************************";
	while(!inputfile.eof()) {
		while (inputline.find(seek_string) == std::string::npos) 
			getline(inputfile,inputline);
		while(inputfile.peek() == ' ') inputfile.get(); // Skip blank space
		// Break for first grid
		if (inputline.find("GRID_DELIMIT") != std::string::npos) break;
		char nextchar = inputfile.peek();
		//bool hascoords = nextchar != '*' && nextchar != ' ' && nextchar != '\n';
		bool hascoords = isalnum(nextchar);
		if (hascoords) {
			//if(agn::debug) std::cout << "Has coords.\n";
			while (!isdigit(inputfile.peek())) inputfile.get();
			inputfile >> xy.first;
			while (!isdigit(inputfile.peek())) inputfile.get();
			inputfile >> xy.second;
			std::pair<double,double> newentry (xy);
			coordlist.push_back(newentry);
			getline(inputfile,inputline);
		}
		else {
			getline(inputfile,inputline);
			while (inputline.find(seek_string) == std::string::npos) 
				getline(inputfile,inputline);
		}
	}
	if(agn::debug) std::cout
		<< "Reached first grid. "
		<< coordlist.size()
		<< " coordinate pairs found.\n";
	agn::cloudy_grid grid;
	agn::gridcoordlist::iterator coords = coordlist.begin();
	while(grid.size() < coordlist.size()) {
		agn::cloudy_result point;
		point.hden = coords->first;
		point.phi = coords->second;

		if(agn::debug) std::cout
			<< "Grabbing header and cautions.";
		seek_string = "Intrinsic line intensities";
		std::string header="";
		std::list<std::string> cautions;
		while (inputline.find(seek_string) == std::string::npos) {
			getline(inputfile,inputline);
			header.append(inputline);
			header.append("\n");
			if(inputline[1] == 'C' && inputline[2] == '-') cautions.push_back(inputline);
		}
		point.header = header;
		point.cautions = cautions;
		std::stringstream headerstr;
		headerstr << header;
		std::string headerword="";
		headerstr.clear();
		headerstr.seekg(0);
		if(agn::debug) std::cout
			<< " Grabbing iterations.";
		int iterations;
		while(!headerstr.eof()) {
			headerstr >> headerword;
			if(headerword == "Iteration") {
				headerstr >> iterations;
				break;
			}
		}
		point.iterations = iterations;
		headerstr.clear();
		headerstr.seekg(0);
		if(agn::debug) std::cout
			<< " Grabbing colden.";
		double colden;
		while(!headerstr.eof()) {
			headerstr >> headerword;
			if(headerword == "column") {
				headerstr >> headerword;
				if(headerword == "density") {
					headerstr >> colden;
					break;
				}
			}
		}
		point.colden = colden;
		if(agn::debug) std::cout 
			<< " Grabbing emission lines.";
		agn::seek_to("Emergent line intensities",inputfile);
		std::list<std::string> emergent_line_raw_text;
		getline(inputfile,inputline);
		while (inputline != "") {
			if(inputline.find("..") != std::string::npos) {
				getline(inputfile,inputline);
				continue;
			}
			emergent_line_raw_text.push_back(inputline);
			getline(inputfile,inputline);
		}
		point.emergent_line_raw_text.swap(emergent_line_raw_text);
		std::list<std::string>::iterator linetext_it=point.emergent_line_raw_text.begin();
		std::list<std::string> duplicate_labels;
		int index=0;
		while(linetext_it != point.emergent_line_raw_text.end()) {
			std::string label=(*linetext_it).substr(1,13);
			agn::cloudy_line_data data;
			data.index = ++index;
			std::stringstream values;
			data.radiated_energy = atof((*linetext_it).substr(17,5).c_str());
			data.eq_width = atof((*linetext_it).substr(22,11).c_str());
			if(point.emergent_line_intensity.count(label) == 0) {
				data.has_duplicates = false;
				point.emergent_line_intensity[label] = data;
			}
			else {
				duplicate_labels.push_back(label);
				data.has_duplicates = true;
				point.emergent_line_intensity[label].has_duplicates = true;
				int j=1;
				while(true) {
					std::stringstream newlabel;
					newlabel << label;
					newlabel << " j=" << j;
					if(point.emergent_line_intensity.count(newlabel.str()) != 0) {
						j++;
						continue;
					}
					else {
						point.emergent_line_intensity[newlabel.str()] = data;
						break;
					}
				}
			}
			linetext_it++;
		}
		if(agn::debug) std::cout 
			<< " Grabbing footer.";
		while (inputline == "")
			getline(inputfile,inputline);
		std::string footer="";
		seek_string="GRID_DELIMIT";
		while (inputline.find(seek_string) == std::string::npos){
			footer.append(inputline);
			footer.append("\n");
			getline(inputfile,inputline);
		}
		if(agn::debug) std::cout 
			<< "\nAdding point to grid: "
			<< point 
			<< std::endl;
		grid[*coords] = point;
		coords++;
	}
	if(agn::debug) std::cout
		<< "Grid captured. "
		<< grid.size()
		<< " points compiled.\n";
	return grid;
}



#endif
