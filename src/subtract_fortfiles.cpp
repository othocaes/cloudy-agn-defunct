#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>

int main(int argc, char const *argv[])
{
    std::ifstream table1;
    table1.open(argv[1]);
    std::ifstream table2;
    table2.open(argv[2]);

    std::string line;

    int table1_numlines = 0;
    while (std::getline(table1, line))
        ++table1_numlines;
    table1.clear();
    table1.seekg(0);

    int table_entries = table1_numlines - 2;

//    int table2_numlines = 0;
//    while (std::getline(table1, line))
//        ++table2_numlines;
//
//    int table2_entries = table2_numlines - 2;
//    table2.clear();
//    table2.seekg(0);

    double hden1[table_entries];
    double phi1[table_entries];
    double magnitude1[table_entries];
    double resultant_magnitude[table_entries];

    std::string first_line, second_line, scratch;
    getline(table1,first_line);
    getline(table1,second_line);
    getline(table2,scratch);
    getline(table2,scratch);

    first_line[0] = argv[3][0];
    first_line[1] = argv[3][1];
    first_line[2] = argv[3][2];
    first_line[3] = argv[3][3];

    for (int i=0; i<table_entries; i++) {
        table1 >> hden1[i] >> phi1[i] >> magnitude1 [i];
    }


    int i=0;
    double hden2, phi2, magnitude2;
    for (int i=0; i<table_entries; i++) {
        if ( table2.eof() ) {
            resultant_magnitude[i]=magnitude1[i];
        }
        else {
        table2 >> hden2 >> phi2 >> magnitude2;
        resultant_magnitude[i] = magnitude1[i] - magnitude2;
        }
        if ( resultant_magnitude[i] < 1e-10 )
            resultant_magnitude[i]=1e-10;
    }



    std::cout
            << first_line
            << std::endl
            //<< "HDen   Phi(H)  ScaInten"
            << second_line
            << std::endl;

    for (int i=0; i<table_entries; i++) {
        std::cout
            << std::fixed
            << std::setprecision(3)
            << hden1[i]
            << "  "
            << phi1[i]
            << "  "
            << std::scientific
            << std::setprecision(4)
            << resultant_magnitude[i]
            << std::endl;
    }

    table1.close();
    table2.close();
    return 0;
}