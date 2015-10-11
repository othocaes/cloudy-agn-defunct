#!/bin/bash

script_dir=$( cd $(dirname $0) ; pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@')
basedir=`pwd`

#	Script to prepare input fort files for Dr. Korista's use, starting from
# an unmodified cloudy grid output file.

#	Separate grid output into individual files.
$script_dir/operations/split_grid_output_hden_phi.sh $1
cd gridoutput


# Create fort tables for these sets of emission lines. Multiplets defined below.
Inci_lines="1215.00A 4860.00A"
TOTL_lines="1215.68A"
HI_lines="1215.68A 1025.73A 972.543A 949.749A 937.809A 6562.85A 4861.36A 4340.49A 4101.76A 3970.09A 3889.07A 1.87511m 1.28181m 1.09381m 1.00494m 9545.99A 9229.03A 2.16553m"
HeII_lines="1640.00A 1215.23A 1085.03A 1025.35A 992.439A 972.186A 4686.01A 3203.30A 2733.46A 1.01242m 6560.44A 5411.80A 4859.57A 4541.82A 4338.89A 4200.05A 4100.25A 4025.81A 3968.64A"
HeI_lines="3888.63A 2.05813m 5015.68A 7065.18A 5875.61A 4713.02A 4471.47A 4120.81A 7281.35A 6678.15A 5047.64A 4921.93A 4437.55A 4387.93A"
metal_lines="C  1 1656.00A
TOTL 2326.00A
C  2 2325.00A
C  2 2324.00A
C  2 2329.00A
C  2 2328.00A
C  2 2327.00A
C  2 1335.00A
C  3 977.000A
TOTL 1909.00A
C  3 1910.00A
C  3 1176.00A
TOTL 1549.00A
C  4 1551.00A
C  4 1548.00A
N  1 1200.00A
N  2 2141.00A
N  2 1085.00A
TOTL 1750.00A
N  3 1749.00A
N  3 1747.00A
N  3 1754.00A
N  3 1752.00A
N  3 1751.00A
N  3 991.000A
TOTL 1486.00A
N  4 1486.00A
N  4 765.000A
TOTL 1240.00A
N  5 1243.00A
N  5 1239.00A
6lev 8446.00A
6lev 1304.00A
TOTL 1665.00A
O  3 1661.00A
O  3 1666.00A
O  3 835.000A
TOTL 1402.00A
O  4 1400.00A
O  4 1397.00A
O  4 1407.00A
O  4 1405.00A
O  4 1401.00A
O  4 789.000A
O  5 630.000A
TOTL 1218.00A
O  5 1218.00A
TOTL 1035.00A
O  6 1031.93A
O  6 1037.62A
Ne 5 1141.00A
TOTL 774.000A
Ne 8 770.404A
Ne 8 780.324A
Na 1 5891.94A
Mg 1 2853.00A
TOTL 2798.00A
Mg 2 2795.53A
Mg 2 2802.71A
TOTL 615.000A
Mg10 609.793A
Mg10 624.941A
totl 2665.00A
Al 2 2670.00A
TOTL 1860.00A
Al 3 1855.00A
Al 3 1863.00A
TOTL 2335.00A
Si 2 2334.00A
Si 2 2329.00A
Si 2 2350.00A
Si 2 2344.00A
Si 2 2336.00A
Si 2 1813.99A
Si 2 1531.00A
Si 2 1307.66A
Si 2 1263.32A
Si 3 1207.00A
TOTL 1888.00A
Si 3 1892.00A
PHOT 1895.00A
TOTL 1397.00A
Si 4 1403.00A
Si 4 1394.00A
S  2 1256.00A
S  3 1720.00A
S  3 1197.56A
TOTL 1406.00A
S  4 1405.00A
S  4 1398.00A
S  4 1424.00A
S  4 1417.00A
S  4 1406.00A
TOTL 1198.00A
S  5 1198.00A
S  5 786.473A
S  6 944.524A
S  6 933.380A
Ca2K 3934.00A
Ca2H 3969.00A
Ca2X 8498.00A
Ca2Y 8542.00A
Ca2Z 8662.00A
Fe 2 1500.00A
Fe 2 1.15000m
Fe 2 2500.00A
Fe 2 2300.00A
Fe 2 8900.00A
Fe 2 1786.00A
Fe 3 1125.79A
FeKa 1.78000A"

continuum_lines="4.11370m 2.32440m 1.64313m 1.48022m 1.43649m 1.11867m 8329.68A 8078.61A 6209.66A 4885.36A 3704.34A 3587.67A 2430.05A 2392.03A 2071.34A 2052.68A 1793.44A 1585.12A 1458.33A 1356.96A 1297.29A 1262.79A 1218.08A 1196.33A 1175.06A 1143.64A 1113.22A 1103.23A 1073.86A 1064.34A 1035.92A 1000.00A 925.144A 897.800A 760.023A 701.514A 608.160A 575.146A 511.948A 496.874A 399.153A 910.000A 913.000A 1650.00A 1900.00A 3644.00A 3648.00A 5100.00A 6500.00A 7000.00A 7500.00A 8000.00A 8204.00A 8207.00A 8500.00A 9000.00A 9500.00A 10000.0A 1.05000m 1.10000m"

fortfile_number=10

echo "Aggregating flux (scaled) tables from grid."

echo " "
echo "Characteristics"
echo "---------------"
for wavelength in $Inci_lines; do
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "Inci ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "Inci ${wavelength} -> tmp.scaledtable.${fortfile_number}"
done
for wavelength in $TOTL_lines; do
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "TOTL ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "TOTL ${wavelength} -> tmp.scaledtable.${fortfile_number}"
done




echo " "
echo "H(I) Series"
echo "-------------"
for wavelength in $HI_lines; do
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "H  1 ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "H  1 ${wavelength} -> tmp.scaledtable.${fortfile_number}"
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_inward_line_flux.sh "H  1 ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "Inwd ${wavelength} -> tmp.scaledtable.${fortfile_number}"	
done


echo " "
echo "He(II) Series"
echo "-----------"
for wavelength in $HeII_lines; do
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "He 2 ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "He 2 ${wavelength} -> tmp.scaledtable.${fortfile_number}"
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_inward_line_flux.sh "He 2 ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "Inwd ${wavelength} -> tmp.scaledtable.${fortfile_number}"
done


echo " "
echo "He(I) Series"
echo "----------"

wavelength="1.08299m"
ref_hden="10.25"
ref_phi="20.00"
num_components=`${script_dir}/operations/count_components.sh "TOTL ${wavelength}" $ref_hden $ref_phi`
component_list=`${script_dir}/operations/find_components.sh "TOTL ${wavelength}" $ref_hden $ref_phi`

(( fortfile_number++ ))
${script_dir}/operations/aggregate_table_for_total_line_flux.sh "TOTL ${wavelength}" > tmp.scaledtable.${fortfile_number}
echo  "TOTL ${wavelength} -> tmp.scaledtable.${fortfile_number}"

(( fortfile_number++ ))
${script_dir}/operations/aggregate_table_for_inward_line_flux.sh "TOTL ${wavelength}" > tmp.scaledtable.${fortfile_number}
echo  "Inwd ${wavelength} -> tmp.scaledtable.${fortfile_number}"

comp_wavelength=`echo "$component_list"|sed -n 1p|sed 's#^......\(........\).*$#\1#'`
for component in `seq 1 $num_components`; do
	comp_identifier=`echo "$component_list"|sed -n ${component}p|sed 's#^.\(.............\).*$#\1#'`
	next_wavelength=`echo "$component_list"|sed -n $((component+1))p|sed 's#^......\(........\).*$#\1#'`
	if [[ "$comp_wavelength" == "$next_wavelength" ]]; then
		
		(( fortfile_number++ ))
		${script_dir}/operations/aggregate_table_for_previous_total_line_flux.sh "${comp_identifier}" $((component-1)) > tmp.scaledtable.${fortfile_number}
		echo  "${comp_identifier} -> tmp.scaledtable.${fortfile_number}"
		
		(( fortfile_number++ ))
		${script_dir}/operations/aggregate_table_for_previous_inward_line_flux.sh "${comp_identifier}" $((component-1)) > tmp.scaledtable.${fortfile_number}
		echo  "Inwd ${comp_wavelength} -> tmp.scaledtable.${fortfile_number}"
	else
		
		(( fortfile_number++ ))
		${script_dir}/operations/aggregate_table_for_total_line_flux.sh "${comp_identifier}" $((component-1)) > tmp.scaledtable.${fortfile_number}
		echo  "${comp_identifier} -> tmp.scaledtable.${fortfile_number}"
		
		(( fortfile_number++ ))
		${script_dir}/operations/aggregate_table_for_inward_line_flux.sh "${comp_identifier}" $((component-1)) > tmp.scaledtable.${fortfile_number}
		echo  "Inwd ${comp_wavelength} -> tmp.scaledtable.${fortfile_number}"
	fi
	comp_wavelength=${next_wavelength}
done

for wavelength in $HeI_lines; do
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "He 1 ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "He 1 ${wavelength} -> tmp.scaledtable.${fortfile_number}"
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_inward_line_flux.sh "He 1 ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "Inwd ${wavelength} -> tmp.scaledtable.${fortfile_number}"
done

echo " "
echo "Heavy Elements"
echo "--------------"
num_lines=`echo "$metal_lines"|wc -l|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
for line_index in `seq 1 $num_lines`; do
	line_id=`echo "${metal_lines}"|sed -n ${line_index}p`
	wavelength=`echo "$line_id"|sed 's#.....\(........\).*#\1#'`
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "${line_id}" > tmp.scaledtable.${fortfile_number}
	echo "${line_id} -> tmp.scaledtable.${fortfile_number}"
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_inward_line_flux.sh "${line_id}" > tmp.scaledtable.${fortfile_number}
	echo "Inwd ${wavelength} -> tmp.scaledtable.${fortfile_number}"
done

echo " "
echo "Continua"
echo "--------"
for wavelength in $continuum_lines; do
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "nFnu ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "nFnu ${wavelength} -> tmp.scaledtable.${fortfile_number}"
	
	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "nInu ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "nInu ${wavelength} -> tmp.scaledtable.${fortfile_number}"

	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "InwT ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "InwT ${wavelength} -> tmp.scaledtable.${fortfile_number}"

	(( fortfile_number++ ))
	${script_dir}/operations/aggregate_table_for_total_line_flux.sh "InwC ${wavelength}" > tmp.scaledtable.${fortfile_number}
	echo "InwC ${wavelength} -> tmp.scaledtable.${fortfile_number}"
	
	#(( fortfile_number++ ))
	#${script_dir}/operations/aggregate_table_for_total_line_flux.sh "InwT ${wavelength}" > tmp.totreflcont.${fortfile_number}
	#echo -n "InwT ${wavelength} -> tmp.totreflcont.${fortfile_number}, "
	#${script_dir}/operations/aggregate_table_for_total_line_flux.sh "InwC ${wavelength}" > tmp.increflcont.${fortfile_number}
	#echo "InwC ${wavelength} -> tmp.increflcont.${fortfile_number}"
	#${script_dir}/../bin/output_difference_table tmp.totreflcont.${fortfile_number} tmp.increflcont.${fortfile_number} > tmp.scaledtable.${fortfile_number}
	#echo "tmp.totreflcont.${fortfile_number} - tmp.increflcont.${fortfile_number} -> tmp.scaledtable.${fortfile_number}"
done


num=10
echo " "
echo "Computing relative intensity tables for all scaled tables."
for scaledtable in `echo tmp.scaledtable.*`; do
	num=`echo "$scaledtable"|sed 's#^tmp\.scaledtable\.\(.*\)$#\1#'`
	#(( num++ ))
	newtable=fort.$num
	${script_dir}/../bin/sort_and_normalize_table $scaledtable > $newtable
	echo "$scaledtable -> $newtable"
done

echo " "
echo "Filing and cleaning."
mkdir -p ${basedir}/fortfiles
mv fort.* ${basedir}/fortfiles
cd ${basedir}/fortfiles
tar zcf ${basedir}/fortfiles.tar.gz *
cd ${basedir}

# ${script_dir}/util/cleantmp.sh

echo "Archive generated. Done."
exit 0


