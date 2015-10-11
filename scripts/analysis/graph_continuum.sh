#!/usr/local/bin/bash
script_dir=$( cd $(dirname $0) ; pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@')

continuum_file=$1

echo Producing scatter plot for incident continuum from $continuum_file

cut -f1,2 $continuum_file >> tmp.cont.xy

