#ifndef sed_hpp
#define sed_hpp



#include "agn.hpp"

namespace agn {

//	Continuum domain, step size constant in log space
``const double CONT_MIN_ENERGY=1e-2; // eV
const double CONT_MAX_ENERGY=1e5; // eV
const double CONT_MIN_X=log10(CONT_MIN_ENERGY);
const double CONT_MAX_X=log10(CONT_MAX_ENERGY);
const double CONT_WIDTH_X=CONT_MAX_X - CONT_MIN_X;
const double CONT_MIN_VAL=1e-35;

//	Cloudy's continuum domain, for reference, version 13.3
const double CLOUDY_EMM = 1.001e-8; // in Rydberg
const double CLOUDY_EGAMRY = 7.354e6; // in Rydberg
const double CLOUDY_MIN_EV=CLOUDY_EMM*RYDBERG_UNIT_EV;
const double CLOUDY_MAX_EV=CLOUDY_EGAMRY*RYDBERG_UNIT_EV;

const double IN_EV_2500A=12398.41929/2500;



//	SEDs are represented by 2d histogram tables.
struct sed_table {
	std::string header;
	table_1d value;
};

class sed_pow_law {
public:
	// Continuum output functions
	// Returns histogram with n bins evenly space in log space
	sed_table histogram_table(int n);

	// Argument is photon energy in eV
	double sed(double hnu);
	double eval_uv(double hnu);
	double eval_xray(double hnu);

	//	Determined differently to be of use as the
	// xray coefficient.
	double SED_at_2KeV();

	// Continuum shape arguments
	double _T; //TCut
	double _alpha_ox;
	double _alpha_x;
	double _alpha_uv;
	double _cutoff_uv_rydberg;
	double _cutoff_xray_rydberg;
	double _log_radius_in_cm;

	// Derived values
	double _cutoff_uv_eV; // IRCut
	double _cutoff_xray_eV;	// lowend_cutoff
	double _radius_in_cm;
	double _radius_in_cm_squared;
	double _scaling_factor;
	double _xray_coefficient;

	sed_pow_law (
		double T,
		double alpha_ox,
		double alpha_x,
		double alpha_uv,
		double cutoff_uv_rydberg,
		double cutoff_xray_rydberg,
		double log_radius_in_cm,
		double scaling_factor = 1.0
        
	    // EL[e] model scaling factor
	    // double scaling_factor = 1.39666E44
		);
};

//	Returns coord in eV for given relative coord.
double hnu_at(int i,int n);

//	Takes an SED table as input and returns a string with format:
// '<h*nu>\t<flux>\n' for each energy-flux pair
std::string format_sed_table(sed_table table);

//	Read continuum from file with '<h*nu>\t<flux>\n' formatting.
// Will ignore up to 1 header.
sed_table read_sed_table(std::ifstream& table_file);

//	Does the same but converts hnu from rydberg to eV.
sed_table read_and_convert_sed_table(std::ifstream& table_file);

//	Cloudy takes the SED density as input. This function outputs
// the corresponding SED table's SED density function in the form
// of a cloudy input script "interpolate" command.
std::string cloudy_interpolate_str(sed_table SED);

} // end namespace agn






agn::sed_table agn::read_sed_table(std::ifstream& table_file) {
    sed_table resultant;
    std::string scratch;
    int current_line=0;
    double hnu;
    std::getline(table_file,scratch);
    if(!isdigit(scratch[0])) {
        resultant.header = scratch;
        current_line++;
    }
    while(!table_file.eof()) {
        table_file >> hnu;
        table_file >> resultant.value[hnu];
    }
}


agn::sed_table agn::read_and_convert_sed_table(std::ifstream& table_file) {
    sed_table resultant;
    std::string scratch;
    int current_line=0;
    double hnu_in_ryd,hnu_in_ev,value;
    std::getline(table_file,scratch);
    if(!isdigit(scratch[0])) {
        resultant.header = scratch;
        current_line++;
    }
    int c=0;
    while(!table_file.eof()) {
        //std::cout << c;
        table_file >> hnu_in_ryd;
        hnu_in_ev = hnu_in_ryd*agn::RYDBERG_UNIT_EV;
        table_file >> resultant.value[hnu_in_ev];
        getline(table_file,scratch);
    }
}

std::string agn::format_sed_table(agn::sed_table table) {
    std::stringstream output;
    if (!table.header.empty()) output << table.header;
    output << std::setprecision(5);
    agn::table2d::iterator table_iterator;
    table_iterator=table.value.begin();
    while(table_iterator != table.value.end()) {
        output
            << std::fixed
            << table_iterator->first
            << "\t"
            << std::scientific
            << table_iterator->second
            << "\n";
            table_iterator++;
    }
    return output.str();
}

std::string agn::cloudy_interpolate_str(agn::sed_table table) {
    std::stringstream output;
    agn::table2d::iterator table_iterator = table.value.begin();
    // Lead in to uv bump at slope=2 in log(energy [rydberg]) space
    double energy_in_rydbergs = table_iterator->first
                                / agn::RYDBERG_UNIT_EV;
    double log_uv_bump_start = log10( energy_in_rydbergs );
    double log_lowest_value = log10(table_iterator->second
                                    / table_iterator->first);
    double log_min_energy = log10(agn::CLOUDY_EMM) 
                            - 1;
    double log_SED_density =  log_lowest_value 
                                - 2*(log_uv_bump_start 
                                - log_min_energy);
    if ( log_SED_density < 1e-36 ) log_SED_density = 1e-36;
    output
        << "interpolate ("
        << pow(10,log_min_energy)
        << " "
        << log_SED_density
        << ")";
    int count=0;

    while(table_iterator != table.value.end()) {
        energy_in_rydbergs = table_iterator->first 
                                    / agn::RYDBERG_UNIT_EV;
        double log_SED_density = log10( table_iterator->second
                                        / table_iterator->first);
        if ((count%5)==0) output << "\n" << "continue ";
        else output << " ";
        output
            << "(" 
            << energy_in_rydbergs
            << " " 
            << log_SED_density
            << ")";
        count++;
        table_iterator++;
    }
    // Trail off at slope=-2 in log(energy [rydberg]) space
    while ( energy_in_rydbergs < agn::CLOUDY_EGAMRY ) {
        double log_energy = log10(energy_in_rydbergs);
        energy_in_rydbergs = pow(10,log_energy+1);
        log_SED_density -= 2;
        output
            << "("
            << energy_in_rydbergs
            << " "
            << log_SED_density
            << ")";
    }
    return output.str();
}



#endif