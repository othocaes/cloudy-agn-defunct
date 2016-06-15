#!/bin/bash

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
        mkdir -p uninterpolated
        mv $fortfile uninterpolated
        $script_dir/../bin/interpolation_fix uninterpolated/$(basename $fortfile) $fortfile
    else
        $script_dir/../bin/interpolation_fix $fortfile $(basename $fortfile)
    fi
    echo ""
done
exit 0
