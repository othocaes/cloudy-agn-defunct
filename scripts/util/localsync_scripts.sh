#!/bin/bash

script_dir=$( cd $(dirname $0) ; pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@')

date > $script_dir/refresh_date

scp -ro port=17940 caes@fulgeo.shadydealings.net:/usr/local/sfs/store/science/agn/scripts/* $script_dir/
chmod -R 700 $script_dir/*


