#!/bin/bash

# This script not generalized. Use with caution.

mkdir fortfiles
cd fortfiles
../../../../../bin/create_fort_files ../mpi_grid.out ../../../../../reference/linelist
../../../../../scripts/meta/bulk_interpolation_fix.sh fort.* >> interpolation_report
tar cf ../fortfiles_4thdex_ngc5548_mehdipour_cldn24.tar fort.*
cd ..
gzip fortfiles_4thdex_ngc5548_mehdipour_cldn24.tar
echo done