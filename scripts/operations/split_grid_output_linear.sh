#!/bin/bash

cloudy_output=$1

grid_delimits=`grep -n 'GRID_DELIMIT' $cloudy_output|sed 's#^\([0-9]*\):.*$#\1 #'`


echo Information Captured
echo ====================
echo scanning cloudy output file $cloudy_output
echo grid_delimits: $grid_delimits

grid_number=0
interval_origin=0

for line_number in $grid_delimits; do
	if [[ $interval_origin == 0 ]]; then
		interval_origin=$line_number
		continue
	fi
	tmpfile=tmp.out.$grid_number
	touch $tmpfile
	echo lines $interval_origin to $line_number to file $tmpfile
	sed -n ${interval_origin},${line_number}p $cloudy_output >> $tmpfile

	interval_origin=$line_number
	(( grid_number++ ))
done



