#ifndef spectral_lines_hpp
#define spectral_lines_hpp

#include "agn.hpp"
#include "png++-0.2.5/gray_pixel.hpp"
#include "png++-0.2.5/image.hpp"


namespace agn {

const double EQWIDTH_MIN_VAL = 1E-7;
const double EQWIDTH_MIN_VAL_LOG = -7;
const double SIGMA_THRESHOLD_MULTIPLIER = 1.0;
const double RATIO_THRESHOLD_MULTIPLIER = .06;

typedef std::list<std::string> line_list;
line_list read_line_list(std::ifstream&);

// Emission line flux contours are represented by a singly-nested sorted map.
struct eqwidth_table {
	std::string header[2];
	table2d value;
};
bool is_zero(const eqwidth_table&);
eqwidth_table read_eqwidth_table(std::ifstream& table_file);
std::string format_eqwidth_table(eqwidth_table table);
std::string format_eqwidth_table_slice(eqwidth_table table,iterator2d x);
gridcoordlist find_outliers(eqwidth_table table);
gridcoordlist known_outliers();
eqwidth_table smooth(eqwidth_table&, gridcoordlist coordlist);
std::list<eqwidth_table> compile_eqwidth_tables(cloudy_grid,line_list,double = 1.0);
// Operator<< prints table formatted for a "fort file".
std::ostream& operator<< (std::ostream&,eqwidth_table);

} // end namespace agn


































// Definitions

bool agn::is_zero(const eqwidth_table& table_to_check) {
    agn::table2d::const_iterator x_it = table_to_check.value.begin();
    while (x_it != table_to_check.value.end()) {
        agn::table1d::const_iterator y_it= x_it->second.begin();
        while (y_it != x_it->second.end()) {
            if (y_it->second != EQWIDTH_MIN_VAL)
                return false;
                y_it++;
        }
        x_it++;
    }
    return true;
}


agn::eqwidth_table agn::read_eqwidth_table(std::ifstream& table_file) {
    agn::eqwidth_table resultant;
    double hden, phi, flux;
    int number_of_lines = 0;
    std::string null;
    while (std::getline(table_file, null))
        ++number_of_lines;
    int table_entries = number_of_lines - 2;
    table_file.clear();
    table_file.seekg(0);
    getline(table_file,resultant.header[0]);
    getline(table_file,resultant.header[1]);
    for (int i=0; i<table_entries; i++) {
        table_file >> hden >> phi >> flux;
        resultant.value[hden][phi]=flux;
    }
    return resultant;
}

std::string agn::format_eqwidth_table(agn::eqwidth_table table) {
    std::stringstream output;
    output
        << table.header[0]
        << "\n"
        << table.header[1]
        << "\n";
    agn::iterator2d hden_it = table.value.begin();
    while(hden_it != table.value.end()) {
        agn::iterator1d phi_it = table.value[hden_it->first].begin();
        while(phi_it != table.value[hden_it->first].end()) {
            output
                << std::fixed
                << std::setprecision(3)
                << hden_it->first
                << "  "
                << phi_it->first
                << "  "
                << std::scientific
                << std::setprecision(4)
                << phi_it->second
                << "\n";
            phi_it++;
        }
        hden_it++;
    }
    return output.str();
}

std::string agn::format_eqwidth_table_slice(agn::eqwidth_table table,agn::iterator2d fixed) {
    std::stringstream output("");
//    output
//        << table.header[0]
//        << " - constant="
//        << fixed->first
//        << "\n";
    iterator1d x=fixed->second.begin();
    while(x!=fixed->second.end()) {
        output
            << std::fixed
            << std::setprecision(3)
            << x->first
            << "\t"
            << std::scientific
            << std::setprecision(4)
            << x->second
            << "\n";
        x++;
    }
    return output.str();
}


std::ostream& agn::operator<< (std::ostream& outstream, agn::eqwidth_table table) {
	outstream
		<< table.header[0]
		<< std::endl
		<< table.header[1]
		<< std::endl;
	agn::table2d::iterator x_it = table.value.begin();
	while (x_it != table.value.end()) {
		agn::table1d::iterator y_it= x_it->second.begin();
		while (y_it != x_it->second.end()) {
			outstream.width(6);
			outstream
				<< std::fixed
				<< std::setprecision(3)
				<< x_it->first
				<< "\t"
				<< y_it->first
				<< "\t"
				<< std::scientific
				<< std::setprecision(4)
				<< y_it->second
				<< "\n";
				y_it++;
		}
		x_it++;
	}
	return outstream;
}


agn::line_list agn::read_line_list(std::ifstream& inputfile) {
	line_list list;
	inputfile.clear();
	inputfile.seekg(0);
	std::string line,label;
	while(!inputfile.eof()) {
		getline(inputfile,line);
		if (line[0] == '#' || line.size() < 2) continue;
		if (line[0] == ' ' && line[1] == ' ') continue;
		if (line[0] == ' ')
			label = line.substr(1,18);
		else
			label = line.substr(0,18);
        // above is only good for c17?
		if(agn::debug) std::cout
			<< "Adding label "
			<< label
			<< " to capture list.\n";
        std::string applabel = label.substr(4);
        if (label.substr(0,4).compare(std::string("nFnu")) == 0) {
            list.push_back(std::string("nFnu").append(applabel));
            list.push_back(std::string("nInu").append(applabel));
            list.push_back(std::string("InwT").append(applabel));
            list.push_back(std::string("InwC").append(applabel));
            continue;
        }
		list.push_back(label);
        list.push_back(std::string("Inwd").append(applabel));
	}
	return list;
}


std::list<agn::eqwidth_table> agn::compile_eqwidth_tables(agn::cloudy_grid grid,agn::line_list line_labels,double scale_factor) {
	std::list<eqwidth_table> table_list_eq_width,table_list_radiated_energy;
	agn::line_list::iterator line_label_it = line_labels.begin();
	while (line_label_it != line_labels.end()) {
		double x,y;
		agn::cloudy_line_data data;
		agn::eqwidth_table new_table;
        if(agn::debug) std::cout
            << "Processing label "
            << *line_label_it
            << std::endl;
        std::stringstream header0;
        header0
            << *line_label_it
            << "  relative to Inci 1215.00A scaled to "
            << scale_factor
            << "A.";
        new_table.header[0] = header0.str();
		new_table.header[1] = "Hden   Phi(H)  Eq_Width (A)";
		agn::cloudy_grid::iterator result_it = grid.begin();
		while (result_it != grid.end()) {
			x = result_it->first.first;
			y = result_it->first.second;
			std::string label = *line_label_it;
			if ( result_it->second.emergent_line_intensity.count(label) == 0 ) {
				data.radiated_energy=-35.0;
				data.eq_width=0.0;
			}
			else {
				data = result_it->second.emergent_line_intensity[label];
				data.eq_width /= scale_factor;
				if(agn::line_debug) std::cout
					<< "Added  "
					<< std::setprecision(2)
					<< std::fixed
					<< x
					<< ", "
					<< y
					<< " -- "
					<< std::scientific
					<< data.radiated_energy
					<< ", "
					<< data.eq_width
					<< "\n";
				if ( data.has_duplicates ) {
					//	This needs to be expanded to catch all duplicate values j=1,2,etc.
					// For now, just blindly use the value the program finds...
				}
			}
			if ( data.radiated_energy < EQWIDTH_MIN_VAL_LOG || data.eq_width < EQWIDTH_MIN_VAL) {
				data.radiated_energy = EQWIDTH_MIN_VAL_LOG;
				data.eq_width = EQWIDTH_MIN_VAL;
			}
			new_table.value[x][y] = data.eq_width;
			result_it++;
		}
		table_list_eq_width.push_back(new_table);
		line_label_it++;
	}
	return table_list_eq_width;
}


#endif