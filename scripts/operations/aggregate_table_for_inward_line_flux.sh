#!/bin/bash

line_id=$1
wavelength=`echo "$line_id"|sed 's#^.....\(........\).*$#\1#'`
gridfiles=`echo [0-9]*_[0-9]*`

if [[ $2 ]]; then 
	echo "Inwd ${wavelength} j=$2 relative to Inci 1215.00A scaled to 1215.00"
else
	echo "Inwd ${wavelength}  relative to Inci 1215.00A scaled to 1215.00"
fi
echo "Hden   Phi(H)  ScaInten"
for file in $gridfiles; do
	hden=`echo $file|cut -d_ -f1|sed 's#^\(.....\).*$#\1#'`
	phi=`echo $file|cut -d_ -f2|sed 's#^\(......\).*$#\1#'`
	line_number=`grep -n "$1" $file|tail -n1|sed 's@\([0-9]*\):.*@\1@'`
	value=`sed -n $((line_number+1))p $file|grep "Inwd"|sed 's#^......................\(.........\).*$#\1#'|tail -n1`
	if [[ $value == "" ]]; then
		value=0.0000;
	fi
	echo "$hden  $phi  $value"
done



