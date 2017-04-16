#!/usr/bin/env bash

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
$bin_dir/create_fort_files ../mpi_grid.out $script_dir/../reference/linelist

inwTfortnums=$(grep 'InwT' fort.*|sed 's@^fort\.\([0-9]\+\):InwT.*@\1@'|sort -nr)
echo "Generating inward diffuse tables for $(echo "$inwTfortnums"|wc -l) coordinates."
echo -n "Writing InwD tables to fortfiles:"
for inwTnum in $(echo $inwTfortnums|sort -r)
do
    inwCnum=$(($inwTnum + 1))
    inwDnum=$(($inwCnum + 1))
    allfortnums=$(echo fort.*|sed 's@fort\.\([0-9]\+\)@\1@g')
    fortnumstoproc=""
    for fortnum in $allfortnums
    do
        if (( $fortnum > $inwCnum ))
        then
            fortnumstoproc="$fortnum $fortnumstoproc"
        fi
    done
    # echo "Shifting $( echo "$fortnumstoproc"|wc -w ) files."
    for fortnum in $fortnumstoproc
    do
        newnum=$(( $fortnum + 1 ))
        mv fort.$fortnum fort.$newnum
    done
    echo -n " $inwDnum"
    $bin_dir/subtract_fortfiles fort.$inwTnum fort.$inwCnum InwD > fort.$inwDnum
done
echo ""

echo "Applying interpolative smoothing."
$script_dir/operations/bulk_interpolation_fix.sh fort.* > interpolation_report

tar cf ngc5548_mehdipour_$(basename ${base_dir})_${cldndir}.tar *
gzip ngc5548_mehdipour_$(basename ${base_dir})_${cldndir}.tar
mv ngc5548_mehdipour_$(basename ${base_dir})_${cldndir}.tar.gz $base_dir
