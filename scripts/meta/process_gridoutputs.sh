#!/usr/bin/env bash

script_dir=$(
    cd $(dirname $0) ;
    pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@'
    )
base_dir=`pwd`
bin_dir="$script_dir/../bin"

for dir in $(echo cldn*)
do
    echo "Processing ${dir}."
    $script_dir/operations/package_tables.sh $dir
done