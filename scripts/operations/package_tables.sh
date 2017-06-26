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

griddir=$1
grid_id=$(echo ${griddir}|sed 's@^\.\/\(.*\)@\1@'|sed 's@.grids@@'|sed 's@\/@.@g')

if [[ -e "${grid_id}.tar.gz" ]]; then
    echo "package exists"
    exit 5
fi

cd $griddir
mkdir -p fortfiles
cd fortfiles
# pwd

echo "Directory ready. Calling fort file creation."
$bin_dir/create_fort_files ../mpi_grid.out $script_dir/../reference/linelist.c17

inwTfortnums=$(grep 'InwT' fort.*|sed 's@^fort\.\([0-9]\+\):InwT.*@\1@'|sort -nr)
echo "Computing inward diffuse tables for $(echo "$inwTfortnums"|wc -l) coordinates."
echo -n "Writing InwD tables to fortfiles and shifting files:"
count=0
for inwTnum in $(echo $inwTfortnums|sort -r)
do
    inwCnum=$(($inwTnum + 1))
    inwDnum=$(($inwCnum + 1))
    allfortnums=$(ls fort.*|sed 's@fort\.\([0-9]\+\)@\1@g'|sort -n)
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
    #echo -n " $inwDnum"
    $bin_dir/subtract_fortfiles fort.$inwTnum fort.$inwCnum InwD > fort.$inwDnum
    echo -ne "\033[2K" ; printf "\r"
    echo -n "Writing InwD tables to fortfiles and shifting files: $(( ++count ))"
done
echo ""

inwCfortnums=$(grep 'InwC' fort.*|sed 's@^fort\.\([0-9]\+\):InwC.*@\1@'|sort -nr)
echo "Computing total diffuse tables for $(echo "$inwCfortnums"|wc -l) coordinates."
echo -n "Writing nFnD tables to fortfiles and shifting files: 0"
count=0
for inwCnum in $(echo $inwCfortnums|sort -r)
do
    nFnunum=$(($inwCnum - 3))
    nFnDnum=$(($inwCnum + 2))
    allfortnums=$(ls fort.*|sed 's@fort\.\([0-9]\+\)@\1@g'|sort -n)
    fortnumstoproc=""
    for fortnum in $allfortnums
    do
        if (( $fortnum >= $nFnDnum ))
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
    
    #echo -n " $nFnDnum"
    $bin_dir/subtract_fortfiles fort.$nFnunum fort.$inwCnum nFnD > fort.$nFnDnum
    echo -ne "\033[2K" ; printf "\r"
    echo -n "Writing nFnD tables to fortfiles and shifting files: $(( ++count ))"
done
echo ""

echo "Applying interpolative smoothing."
$script_dir/operations/bulk_interpolation_fix.sh fort.* > interpolation_report


grid_id=$(echo ${griddir}|sed 's@^\.\/\(.*\)@\1@'|sed 's@.grids@@'|sed 's@\/@.@g')
echo "Saving $grid_id grid"
tar cf fortfiles_${grid_id}.tar *
gzip fortfiles_${grid_id}.tar
mv fortfiles_${grid_id}.tar.gz $base_dir

cd $base_dir