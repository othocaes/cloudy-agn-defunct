#!/bin/bash

#   This script converts a cloudy incident continuum's photon
# energy unit from Rydberg to eV. Best if continuum is
# parsed to a raw table, first. Photon energy needs to be
# in the first column. The bash program bc is required.



continuum_file_orig=$1
continuum_file_new=hnuineV.$continuum_file_orig



while read photon_energy_in_ryd everything_else; do
    # 1 Ryd = 13.60569253 eV
    photon_energy_in_eV=`bc <<< "$photon_energy_in_ryd * 13.60569253"`
done <$continuum_file_orig



