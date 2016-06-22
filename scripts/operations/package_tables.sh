#!/usr/local/bin/bash

# This still isn't really generalized. Need to go to a
# specific directory of runs to run this. Feed it a list
# of column densities.

script_dir=$(
    cd $(dirname $0) ;
    pwd -P |sed 's@^\(.*\)/scripts.*@\1/scripts@'
    )
base_dir=`pwd`
bin_dir="$script_dir/../bin"

cldndir=$1
cd $cldndir
mkdir -p fortfiles
cd fortfiles
# pwd

echo "Directory ready. Calling fort file creation."
# $bin_dir/create_fort_files ../mpi_grid.out $script_dir/../reference/linelist

allfortnums=$(echo fort.*|sed 's@fort\.\([0-9]\+\)@\1@g')
inwTfortnums=$(grep 'InwT' fort.*|sed 's@^fort\.\([0-9]\+\):InwT.*@\1@')
echo "Generating inward diffuse tables for $(echo "$inwTfortnums"|wc -l) coordinates."
for inwTnum in $inwTfortnums
do
    inwCnum=$(($inwTnum + 1))
    inwDnum=$(($inwCnum + 1))
    fortnumstoproc=""
    for fortnum in $allfortnums
    do
        if [[ $fortnum > $inwCnum ]]
        then
            fortnumstoproc="$fortnum $fortnumstoproc"
        fi
    done
    echo "Shifting $( echo "$fortnumstoproc"|wc -w ) files."
    for fortnum in $fortnumstoproc
    do
        newnum=$(( $fortnum + 1 ))
        mv fort.$fortnum fort.$newnum
    done
    echo "Writing inwD table to fort.$inwDnum"
    $bin_dir/subtract_fortfiles fort.$inwTnum fort.$inwCnum > fort.$inwDnum
done

echo "Applying interpolative smoothing."
$script_dir/operations/bulk_interpolation_fix.sh fort.* > interpolation_report

tar cf ngc5548_mehdipour_4thdexgrid_${cldndir}.tar *
gzip ngc5548_mehdipour_4thdexgrid_${cldndir}.tar
mv ngc5548_mehdipour_4thdexgrid_${cldndir}.tar.gz $base_dir/..


