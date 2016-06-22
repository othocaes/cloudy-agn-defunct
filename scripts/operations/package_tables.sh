cldn=$1
cd cldn${cldn}
mkdir -p fortfiles
cd fortfiles
# pwd
../../../../../bin/create_fort_files ../mpi_grid.out ../../../../../reference/linelist
tar cf ngc5548_mehdipour_4thdexgrid_cldn${cldn}.tar *
gzip ngc5548_mehdipour_4thdexgrid_cldn${cldn}.tar
mv ngc5548_mehdipour_4thdexgrid_cldn${cldn}.tar.gz ../..

 
