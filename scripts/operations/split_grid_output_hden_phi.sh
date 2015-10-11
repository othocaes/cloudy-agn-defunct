#!/bin/bash

script_dir=$( cd $(dirname $0) ; pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@')
basedir=`pwd`

echo "Separating grid by hden and phi(H)."


outputdir="gridoutput"
mkdir -p $outputdir; cd $outputdir
cloudy_output="../$1"

echo scanning cloudy output file $1
grid_delimits=`grep -n 'GRID_DELIMIT' $cloudy_output|sed 's#^\([0-9]*\):.*$#\1 #'`
echo $( echo $grid_delimits|wc -w|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#' ) grid delimiters found.

metafile=tmp.out.meta
hdenfile=tmp.out.hden_list
phifile=tmp.out.phi_list

grid_number=0
interval_origin=0

echo "Separating grids."
for line_number in $grid_delimits; do
	if [[ $interval_origin == 0 ]]; then
		sed -n 1,${line_number}p $cloudy_output > $metafile
		interval_origin=$line_number
		echo Metafile collected.
		continue
	fi
	tmpfile=tmp.out.$grid_number
	touch $tmpfile
	sed -n ${interval_origin},${line_number}p $cloudy_output > $tmpfile
	interval_origin=$line_number
	(( grid_number++ ))
done

echo "Output organized by linear index."

params_start=`grep -n "MPI grid mode " $metafile|cut -d':' -f1`
endline=`wc -l $metafile|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
echo Reading parameters from lines $params_start to $endline
sed -n ${params_start},${endline}p $metafile|grep "HDEN="|sed 's#^[^0-9]*\([0-9.]*\) LOG.*$#\1#' > ${hdenfile}
sed -n ${params_start},${endline}p $metafile|grep "phi(h) "|sed 's#^[^0-9]*\([0-9.]*\) LOG.*$#\1#' > ${phifile}
num_params=`wc -l ${hdenfile}|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`

#	 Note that this program assumes for each hden, phi was varied
# over its domain. Come to think of it, however, the program should
# remain consistent, either way.
files_processed=0
for tmpfile in `echo tmp.out.[0-9]*`; do
	linear_index=`echo $tmpfile|sed 's#^[^0-9]*\([0-9]*\)[^0-9]*$#\1#'`
	(( linear_index++ ))
	hden=`sed -n ${linear_index}p ${hdenfile}`
	phi=`sed -n ${linear_index}p ${phifile}`
	cp $tmpfile ${hden}_${phi}
	(( files_processed++ ))
done
echo Output organized by grid parameters.
echo $files_processed grid events were processed. This should be 29 x 29.


${script_dir}/util/cleantmp.sh
cd ${basedir}


