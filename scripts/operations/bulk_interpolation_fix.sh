#!/bin/bash

# This script not generalized for the project, but setup to work with package_table.sh and process_gridoutputs.sh as of June 2016.

script_dir=$( cd $(dirname $0) ; pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@')
basedir=`pwd`

if [[ -x $script_dir/../bin/interpolation_fix ]]
then
    echo "Binary available. Proceeding."
else
    echo "Binary unavailable. Exiting."
    exit 9
fi

for fortfile in $@
do
    if [[ -e $(basename $fortfile) ]]
    then
        mkdir -p raw
        mv $fortfile raw
        $script_dir/../bin/interpolation_fix raw/$(basename $fortfile) $fortfile
    else
        $script_dir/../bin/interpolation_fix $fortfile $(basename $fortfile)
    fi
    echo ""
done

mkdir -p interpolated_consthden
mv fort.* interpolated_consthden

exit 0
