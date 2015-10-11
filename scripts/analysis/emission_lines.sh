#!/bin/bash

# find line number for final emergent line intensities

# Find and display these sets of emission lines.
# wavelengths in Angstroms

Ba_set="6562.85A 4861.36A 4340.49A 4101.76A 3970.09A"
Ly_set="1215.68A 1025.73A 972.543A 949.749A"
Pa_set="1.28181m 1.09381m 1.00494m 9545.99A 9229.03A"
HeII_set="1640.00A 4686.01A"
HeI_set="5875.61A 1.08299m 6678.15A 4471.47A 3888.63A 5015.68A"
C_IV_set="1549.00A 1551.00A 1548.00A"
Mg_II_set="2798.00A"


cloudy_output_file=$1
emergent_lines_start=`grep -n "Emergent line intensities" $cloudy_output_file|tail -n1|sed 's@\([0-9]*\):.*@\1@'`
total_lines=`wc -l $cloudy_output_file|sed 's#^[^0-9]*\([0-9]*\) .*$#\1#'`


echo "Requested Emergent Emission Line Fluxes"
echo "======================================"
echo "Run parameters"
echo "--------------"
HDEN=`grep 'HDEN=' $cloudy_output_file|tail -n1|sed 's#^[^0-9]*\([0-9.]*\) LOG.*$#\1#'`
PHIH=`grep 'phi(h) ' $cloudy_output_file|tail -n1|sed 's#^[^0-9]*\([0-9.]*\) LOG.*$#\1#'`
echo "hden = ${HDEN}"
echo "phi(h) = ${PHIH}"
echo " "

echo "Unit Intensity"
echo "--------------"
sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep 1.0000
echo " "


# Extract hydrogen sets
echo -n "Balmer Series"
list_size=`echo $Ba_set|wc -w|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
echo " (seeking ${list_size})"
echo "-------------"
for emission_line in $Ba_set; do
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "H  1 ${emission_line}"
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "Inwd ${emission_line}"
done
echo " "

echo -n "Lyman Series"
list_size=`echo $Ly_set|wc -w|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
echo " (seeking ${list_size})"
echo "------------"
for emission_line in $Ly_set; do
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "H  1 ${emission_line}"
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "Inwd ${emission_line}"
done
echo " "

echo -n "Paschen Series"
list_size=`echo $Pa_set|wc -w|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
echo " (seeking ${list_size})"
echo "------------"
for emission_line in $Pa_set; do
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "H  1 ${emission_line}"
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "Inwd ${emission_line}"
done
echo " "


echo -n "HeII Selected Lines"
list_size=`echo $HeII_set|wc -w|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
echo " (seeking ${list_size})"
echo "-------------------"
for emission_line in $HeII_set; do
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "He 2 ${emission_line}"
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "Inwd ${emission_line}"
done
echo " "

echo -n "HeI Selected Lines"
list_size=`echo $HeI_set|wc -w|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
echo " (seeking ${list_size})"
echo "------------------"
for emission_line in $HeI_set; do
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "He 1 ${emission_line}"
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "Inwd ${emission_line}"
done
echo " "

echo -n "C_IV Selected Lines"
list_size=`echo $C_IV_set|wc -w|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
echo " (seeking ${list_size})"
echo "-------------------"
for emission_line in $C_IV_set; do
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "${emission_line}"
done
echo " "

echo -n "Mg_II Selected Lines"
list_size=`echo $Mg_II_set|wc -w|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
echo " (seeking ${list_size})"
echo "--------------------"
for emission_line in $Mg_II_set; do
	sed -n $emergent_lines_start,${total_lines}p $cloudy_output_file|grep "${emission_line}"
done
echo " "