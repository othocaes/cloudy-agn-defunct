#include "agn.hpp"



int main(int argc, char const *argv[])
{

    std::cout 
        << "Setting up environment.\n";

    bool debug=true;

    //  Create 2d table using n bins, linear values of SED. The
    // agn em source class has a function for this. A 
    // std::map<double,double> represents the table.
    int n = 1000;
    agn::sed_table SED;

    const char* table_filename = "agn_source_table";
    const char* debug_filename = "agn_source_debug";
    const char* cloudyscript_filename = "agn_source_cloudyscript";

    std::ofstream table_file(   table_filename,
                                std::ofstream::out
                            );
    std::ofstream debug_file(   debug_filename,
                                std::ofstream::out
                            );
    std::ofstream cloudyscript_file(cloudyscript_filename,
                                    std::ofstream::out
                                    );

    if(debug) std::cout 
        << "Debug mode.\n";


    std::cout 
        << "Creating agn sed object.\n";

    // agn em source spectrum arguments
    double  T=4e6,
            alpha_ox = -1.20,
            alpha_x = -0.670,
            alpha_uv = -1.30,
            cutoff_uv_rydberg = .25,
            cutoff_xray_rydberg = .1,
            log_radius_in_cm=16.7272;;

    agn::sed_pow_law agnsource(
                            T,
                            alpha_ox,
                            alpha_x,
                            alpha_uv,
                            cutoff_uv_rydberg,
                            cutoff_xray_rydberg,
                            log_radius_in_cm
                            );

    if(debug) debug_file
                    << "cutoff_uv_eV: "
                    << agnsource._cutoff_uv_eV
                    << "\n"
                    << "cutoff_xray_eV: "
                    << agnsource._cutoff_xray_eV
                    << "\n"
                    << "xray coefficient: "
                    << agnsource._xray_coefficient
                    << "\n\n";



    std::cout
        << "Evaluating relative spectral intensity for "
        << n
        << " photon energy bins.\n";

    SED = agnsource.histogram_table(n);

    std::cout   
        << "Printing SED table to file " 
        << table_filename 
        << "\n";

    table_file << agn::format_sed_table(SED);

    std::cout
        << "Printing CLOUDY interpolate command syntax to file "
        << cloudyscript_filename 
        << "\n";

    cloudyscript_file << agn::cloudy_interpolate_str(SED);

    std::cout
        << "Closing files. Goodbye.\n";

    debug_file.close();
    cloudyscript_file.close();
    table_file.close();
    return 0;
}












double agn::hnu_at(int i,int n) {
    double relative_coord=(double)(i)/n;
    double x_coord = relative_coord*CONT_WIDTH_X + CONT_MIN_X;
    return pow(10,x_coord);
}

agn::sed_table agn::sed_pow_law::histogram_table(int n){
    agn::sed_table output;
    double max=0,min=1,hnu;
    for(int i=0; i<n; i++) {
        hnu = hnu_at(i,n);
        output.value[hnu] = this->sed(hnu);
        if (output.value[hnu] > max) max = output.value[hnu];
        if (output.value[hnu] < min) min = output.value[hnu];
    }
    // Add a final point at 100 KeV
    hnu = 1e5;
    output.value[hnu] = this->sed(hnu);
    return output;
}

double agn::sed_pow_law::sed(double hnu) {
    double magnitude=0.0;
    magnitude += this->eval_uv(hnu);
    magnitude += this->eval_xray(hnu);
    if (magnitude < agn::CONT_MIN_VAL) return agn::CONT_MIN_VAL;
    return magnitude;
}
double agn::sed_pow_law::eval_uv(double hnu) { 
    double bigbump_kT = _T
                        * agn::BOLTZMANN_CONST;
    double magnitude =  pow(hnu,(1+_alpha_uv))
                    * exp(-(hnu)/bigbump_kT)
                    * exp(-(_cutoff_uv_eV/hnu))
                    * _scaling_factor;
    if (magnitude < agn::CONT_MIN_VAL) return agn::CONT_MIN_VAL;
    return magnitude;
}
double agn::sed_pow_law::eval_xray(double hnu) { 
    return  _xray_coefficient
            * pow(hnu/2000,1+_alpha_x)
            * exp(-_cutoff_xray_eV/hnu)
            * _scaling_factor;
}

double agn::sed_pow_law::SED_at_2KeV() {
    double ELe_at_2500A_no_scale =  eval_uv(IN_EV_2500A)
                                    / _scaling_factor;
    double energy_ratio = 2000/IN_EV_2500A;
    // Returns EL[e] at 2 KeV
    return  ELe_at_2500A_no_scale 
            * pow(energy_ratio,_alpha_ox + 1);
}

agn::sed_pow_law::sed_pow_law (
    double T,
    double alpha_ox,
    double alpha_x,
    double alpha_uv,
    double cutoff_uv_rydberg,
    double cutoff_xray_rydberg,
    double log_radius_in_cm,
    double scaling_factor
    ): 
    _T(T),
    _alpha_ox(alpha_ox),
    _alpha_x(alpha_x),
    _alpha_uv(alpha_uv),
    _cutoff_uv_rydberg(cutoff_uv_rydberg),
    _cutoff_xray_rydberg(cutoff_xray_rydberg),
    _log_radius_in_cm(log_radius_in_cm),
    _scaling_factor(scaling_factor)
    {
        _cutoff_uv_eV = cutoff_uv_rydberg*RYDBERG_UNIT_EV;
        _cutoff_xray_eV = cutoff_xray_rydberg*RYDBERG_UNIT_EV;
        _radius_in_cm = pow(10,log_radius_in_cm);
        _radius_in_cm_squared = _radius_in_cm*_radius_in_cm;
        _xray_coefficient = agn::sed_pow_law::SED_at_2KeV();
}