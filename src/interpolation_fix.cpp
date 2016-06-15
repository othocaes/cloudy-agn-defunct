#include "agn.hpp"
#include "spectral_lines.hpp"
#include "spline.h"

bool print_histogram=true;

int main(int argc, char const *argv[]) {
    std::ifstream eqwidth_table_file;
    eqwidth_table_file.open(argv[1]);
    agn::eqwidth_table jagged_table = agn::read_eqwidth_table(eqwidth_table_file);
    eqwidth_table_file.close();

    std::cout
        << "Interpolation Fix - "
        << jagged_table.header[0]
        << "\n";

    std::cout << "Finding outliers.\n";
    agn::gridcoordlist outliers = agn::find_outliers(jagged_table);
    //agn::gridcoordlist outliers = agn::known_outliers();

    std::cout << "Replacing outliers with interpolated values.\n";
    agn::eqwidth_table smooth_table = agn::smooth(jagged_table,outliers);

    // Output debug info
    agn::gridcoordlist::iterator outlier_it = outliers.begin();
    while(outlier_it != outliers.end()) {
        std::cout
            << std::setprecision(2)
            << std::fixed
            << "Reported smooth at: "
            << outlier_it->first
            << ", "
            << outlier_it->second
            << "\n"
            << std::scientific
            << "Old value: "
            << jagged_table.value[outlier_it->first][outlier_it->second]
            << "\t"
            << "New value: "
            << smooth_table.value[outlier_it->first][outlier_it->second]
            << "\n";
            outlier_it++;
    }
    std::ofstream smoothed_table_file;
    smoothed_table_file.open(argv[2]);
    smoothed_table_file << agn::format_eqwidth_table(smooth_table);
    smoothed_table_file.close();

    return 0;
}

agn::eqwidth_table agn::smooth(agn::eqwidth_table & jagged_table,agn::gridcoordlist coordlist) {
    if(agn::debug) std::cout
        << "Received Coordinate list with "
        << coordlist.size()
        << " items.\n";

    agn::eqwidth_table smooth_table = agn::eqwidth_table(jagged_table);
    agn::gridcoordlist::iterator coord_it = coordlist.begin();
    while(coord_it != coordlist.end() ) {
        double x = coord_it->first;
        double y = coord_it->second;
        if(agn::debug) std::cout
            << "Smoothing at hden="
            << x
            << ", phi="
            << y
            << ":\t";
        agn::table1d slice = agn::table1d(smooth_table.value[x]);

        double oldx = y;
        double oldy = slice[oldx];

        std::vector<double> X;
        std::vector<double> Y;

        // Omit any points in the outlier list.
        std::set<double> omissions;
        agn::gridcoordlist::iterator outlier_it = coordlist.begin();
        while ( outlier_it != coordlist.end() ) {
            if ( (*outlier_it).first == (*coord_it).first ) {
                omissions.insert((*outlier_it).second);
            }
            outlier_it++;
        }

        agn::table1d::iterator slit = slice.begin();
        while ( slit != slice.end() ) {
            std::set<double>::iterator omissions_it = omissions.begin();
            while ( omissions_it != omissions.end() ) {
                if ( slit->first == *omissions_it) {
                    slit++;
                    continue;
                }
                omissions_it++;
            }
            X.push_back(slit->first);
            Y.push_back(slit->second);
            slit++;
        }

        agn::Spline<double,double> interpolator(X,Y);

        slit = slice.find(y);

        double newx = slit->first;
        double newy = interpolator[newx];

        if ( newx != oldx ) {
            std::cerr << "Problem smoothing.\n";
            coord_it++;
            continue;
        }

        smooth_table.value[x][y] = newy;

        if ( agn::debug ) {
            std::cout
                << std::scientific
                << "Smoothed: "
                << newx
                << ", "
                << oldy
                << " -> "
                << newy
                << "\n";
        }

        coord_it++;
    }

    if(agn::debug) std::cout << "\n";
    return smooth_table;
}

agn::gridcoordlist agn::known_outliers() {
    agn::gridcoordlist colden23_181515_diverged_points;
    colden23_181515_diverged_points.push_front(std::make_pair(10.25,20.0 ));
    colden23_181515_diverged_points.push_front(std::make_pair(10.5,20.25 ));
    colden23_181515_diverged_points.push_front(std::make_pair(11.25,20.75));
    colden23_181515_diverged_points.push_front(std::make_pair(11.5,21.0  ));
    colden23_181515_diverged_points.push_front(std::make_pair(11.5,22.0  ));
    colden23_181515_diverged_points.push_front(std::make_pair(11.75,21.25));
    colden23_181515_diverged_points.push_front(std::make_pair(12.0,21.5  ));
    colden23_181515_diverged_points.push_front(std::make_pair(12.5,22.0  ));
    colden23_181515_diverged_points.push_front(std::make_pair(12.5,22.25 ));
    colden23_181515_diverged_points.push_front(std::make_pair(9.75,19.75 ));
    return colden23_181515_diverged_points;
}

agn::gridcoordlist agn::find_outliers(agn::eqwidth_table jagged_table) {
    if (agn::debug) std::cout
        << "Scanning for outliers.\n";
    agn::gridcoordlist outliers;

    // The distribution is
    // parameterised by x=log(jagged_table.hden) and
    // y=log(jagged_table.eqwidth).

    agn::iterator2d hden_it = jagged_table.value.begin();
    while ( hden_it != jagged_table.value.end() ) {
        agn::table1d slice = hden_it->second;
        if( agn::debug ) std::cout
            << "\nhden= "
            << (hden_it->first)
            << ": ";

        // Crawl, checking slope, until a positive slope is found.
        agn::table1d::iterator phi_it = slice.begin();
        double x1,x2,y1,y2,slope;
        std::vector<double> ref_curve_x,ref_curve_y,curve_back_x,curve_back_y;
        double anomalies_start_x,anomalies_end_x;
        x1 = phi_it->first;
        y1 = log10(phi_it->second);
        ref_curve_x.push_back(x1);
        ref_curve_y.push_back(y1);
        phi_it++;
        bool beginning_rise = true;
        while ( phi_it != slice.end() ) {
            x2 = phi_it->first;
            y2 = log10(phi_it->second);
            slope = (y2 - y1)/(x2 - x1);
            if (slope > -0.05 ) {
                if (agn::debug) std::cout
                    << "Found anomaly(x="
                    << x1
                    << ";slope="
                    << slope
                    << "), ";
                if ( beginning_rise ) {
                    if (agn::debug) std::cout
                        << "Found left-wise rise, ";
                    while ( slope > -0.05 ) {
                        x1 = phi_it->first;
                        y1 = log10(phi_it->second);
                        phi_it++;
                        if ( phi_it == slice.end() ) break;
                        x2 = phi_it->first;
                        y2 = log10(phi_it->second);
                        slope = (y2 - y1)/(x2 - x1);
                    }
                    ref_curve_x.clear();
                    ref_curve_y.clear();
                    ref_curve_x.push_back(x1);
                    ref_curve_y.push_back(y1);
                }
                else {
                    ref_curve_x.pop_back();
                    ref_curve_y.pop_back();
                    phi_it++;
                    break;
                }
            }
            if ( slope < -0.05 && ( slice.find(x1) != slice.begin())) beginning_rise = false;
            x1 = x2;
            y1 = y2;
            ref_curve_x.push_back(x1);
            ref_curve_y.push_back(y1);
            phi_it++;
        }

        if ( phi_it == slice.end() ) {
            if (agn::debug) std::cout
                << "Clean";
            hden_it++;
            continue;
        }

        x1 = phi_it->first;
        y1 = log10(phi_it->second);
        curve_back_x.push_back(x1);
        curve_back_y.push_back(y1);
        phi_it++;
        while ( phi_it != slice.end() ) {
            x2 = phi_it->first;
            y2 = log10(phi_it->second);
            slope = (y2 - y1)/(x2 - x1);
            if (slope > 0 ) {
                curve_back_x.clear();
                curve_back_y.clear();
            }
            if ( y2 <= EQWIDTH_MIN_VAL_LOG ) {
                break;
            }
            x1 = x2;
            y1 = y2;
            curve_back_x.push_back(x1);
            curve_back_y.push_back(y1);
            phi_it++;
        }

        if ( curve_back_x.size() < 2 ) {
            if(agn::debug) std::cout
                << "Found too few end points for spline ("
                << curve_back_x.size()
                << ")";
            hden_it++;
            continue;
        }


        if (ref_curve_x.size() > 2) {
            ref_curve_x.erase(ref_curve_x.begin());
            ref_curve_y.erase(ref_curve_y.begin());
            ref_curve_x.pop_back();
            ref_curve_y.pop_back();
        }
        if (curve_back_x.size() > 2) {
            curve_back_x.erase(curve_back_x.begin());
            curve_back_y.erase(curve_back_y.begin());
        }
        anomalies_start_x = *ref_curve_x.rbegin();
        anomalies_end_x = *curve_back_x.begin();

        if (agn::debug) std::cout
            << "Anomalous region between x coords "
            << anomalies_start_x
            << " and "
            << anomalies_end_x
            << ", ";

        std::vector<double>::iterator curve_back_x_it = curve_back_x.begin();
        std::vector<double>::iterator curve_back_y_it = curve_back_y.begin();
        while ( curve_back_x_it != curve_back_x.end() &&
                 curve_back_y_it != curve_back_y.end() ) {
            ref_curve_x.push_back(*curve_back_x_it);
            ref_curve_y.push_back(*curve_back_y_it);
            curve_back_x_it++;
            curve_back_y_it++;
        }

        if (agn::debug) std::cout
            << "Creating Spline with "
            << ref_curve_x.size()
            << " points, ";

        // Creating spline from vectors
        agn::Spline<double,double> testing_spline(ref_curve_x,ref_curve_y);

        // Calculate sigma from the reference curve.
        std::vector<double>::iterator ref_curve_y_it = ref_curve_y.begin();
        double sigma = 0;
        double mean = 0;
        int num = ref_curve_y.size();
        double sum = 0;
        double threshold = 0;
        while ( ref_curve_y_it != ref_curve_y.end() ) {
            mean += *ref_curve_y_it;
            ref_curve_y_it++;
        }
        mean /= num;
        ref_curve_y_it = ref_curve_y.begin();
        while ( ref_curve_y_it != ref_curve_y.end() ) {
            sum += pow(*ref_curve_y_it - mean,2);
            ref_curve_y_it++;
        }
        sigma = sqrt(sum / num);

        // Interpolate all values in anomalies_x range and add to list when value is outside threshold.
        //threshold = sigma*SIGMA_THRESHOLD_MULTIPLIER;
        //if (agn::debug) std::cout
        //    << "using sigma(n="
        //    << num
        //    << ")="
        //    << sigma
        //    << " and threshold="
        //    << threshold
        //    << ", ";
        phi_it = slice.find(anomalies_start_x);
        while (phi_it != (++(slice.find(anomalies_end_x)))) {
            y1 = log10(phi_it->second);
            x1 = phi_it->first;
            y2 = testing_spline[x1];
            //threshold = RATIO_THRESHOLD_MULTIPLIER * (y2 - EQWIDTH_MIN_VAL_LOG);
            threshold = 2*log(1 + RATIO_THRESHOLD_MULTIPLIER);
            double absdiff = sqrt(pow(y2 - y1,2));
            if(agn::debug) std::cout
                << "Checking "
                << "x="
                << x1
                << "("
                << absdiff
                << " > "
                << threshold
                << "), ";
            if ( absdiff > threshold) {
                if(agn::debug) std::cout
                    << "MARKED, ";
                std::pair<double,double> outlier = std::make_pair(hden_it->first,phi_it->first);
                outliers.push_back(outlier);
            }
            phi_it++;
        }
        if(agn::debug) std::cout << "\n";
        hden_it++;
    }

    if (agn::debug) std::cout << "\n";
    return outliers;
}