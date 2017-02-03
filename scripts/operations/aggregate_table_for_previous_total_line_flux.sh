#!/bin/bash

line_id=$1
gridfiles=`echo [0-9]*_[0-9]*`

if [[ $2 ]]; then 
	echo "$1 j=$2 relative to Inci 1215.00A scaled to 1215.00"
else
	echo "$1  relative to Inci 1215.00A scaled to 1215.00"
fi
echo "Hden   Phi(H)  ScaInten"
for file in $gridfiles; do
	hden=`echo $file|cut -d_ -f1|sed 's#^\(.....\).*$#\1#'`
	phi=`echo $file|cut -d_ -f2|sed 's#^\(......\).*$#\1#'`
	line_number=`grep -n "$1" $file|tail -n1|sed 's@\([0-9]*\):.*@\1@'`
	if [[ $line_number == "" ]]; then 
		value=0.0000;
	else
		value=`sed -n $(( line_number - 2 )),$(( line_number - 1 ))p $file|grep "$1"|sed 's#^......................\(.........\).*$#\1#'|head -n1`
	fi
	if [[ $value == "" ]]; then
		value=0.0000;
	fi
	echo "$hden  $phi  $value"
done



