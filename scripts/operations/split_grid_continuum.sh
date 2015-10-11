#!/bin/bash
script_dir=$( cd $(dirname $0) ; pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@')

continuum_file=$1

grid_delimits=`grep -n 'GRID_DELIMIT' $continuum_file|sed 's#^\([0-9]*\):.*$#\1 #'`


echo Parse Grid Continuum
echo ====================
echo Scanning file $continuum_file.
echo `echo $grid_delimits|wc -w` grids found.
#echo Grid delimits: $grid_delimits

grid_index=0
# Skips extra line at beginning of first grid component
interval_origin=1

# Parse the continua for each grid component into a separate file
echo Parsing continua into separate grid components.
for line_number in $grid_delimits; do
    (( interval_origin++ ))
tmpfile=tmp.cont.$grid_index
    #echo lines $interval_origin to $line_number to file $tmpfile
    sed -n ${interval_origin},${line_number}p $continuum_file > $tmpfile
    interval_origin=$line_number
    (( grid_index++ )) 
done

# Parse the final continuum for each grid component and save
grid_component=0
echo "Parsing continua for final iteration of $grid_index grid components."
while [[ $grid_component -lt $grid_index ]]; do
    cont_tmp_file=tmp.cont.$grid_component
    permanent_file=${continuum_file}.$((grid_component++))
    iteration_delimits=`grep -n '###########################' $cont_tmp_file|sed 's#^\([0-9]*\):.*$#\1 #'`
    iterations=`echo $iteration_delimits|wc -w`
    sed -n $((`echo $iteration_delimits|cut -d' ' -f$(( iterations - 1 ))` + 1 )),`echo $iteration_delimits|cut -d' ' -f$(( iterations ))`p $cont_tmp_file > $permanent_file
done



$script_dir/util/cleantmp.sh
echo Done.
