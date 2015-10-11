#!/bin/bash

reference_file=`echo ${2}[0-9]*_${3}[0-9]*`
line_number=`grep -n "$1" ${reference_file}|tail -n1|sed 's#\([0-9]\): TOTL.*#\1#'`
avg_wavelength=`sed -n ${line_number}p ${reference_file}|sed 's#......\(.......\).*#\1#'`
components_found=0
num_potential_components=14
for line_num in `seq $num_potential_components`; do
	line=`sed -n $((line_number + $line_num))p ${reference_file}`
	# echo "$line"
	if [[ `echo $line|grep Inwd` != "" ]]; then continue; fi
	if [[ `echo $line|grep TOTL` != "" ]]; then break; fi
	element_in_line=`echo "$line"|sed 's#^.\(....\).*$#\1#'`
	if [[ ${element} == "" ]]; then element=${element_in_line}; fi
	if [[ ${element} != ${element_in_line} ]]; then continue; fi
	component_wavelength=`echo "$line"|sed 's#......\(.......\).*#\1#'`
	# echo $avg_wavelength $component_wavelength
	if [[ `bc <<< "scale=5; sqrt( ( ( ${avg_wavelength} - ${component_wavelength} ) / ${avg_wavelength} )^2 ) < .015"` == 1 ]]; then
		(( components_found++ ))
	else
		break
	fi
done
echo $components_found
exit 0
