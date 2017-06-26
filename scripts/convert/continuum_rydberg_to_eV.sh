#!/bin/bash

#   This script converts a cloudy incident continuum's photon
# energy unit from Rydberg to eV. Best if continuum is
# parsed to a raw table, first. Photon energy needs to be
# in the first column. The bash program bc is required.



while read -r rydbergs sed1 sed2
do
    # 1 Ryd = 13.60569253 eV
    eV=$(bc <<< "$rydbergs * 13.60569253")
    echo "$eV $sed1 $sed2"
done

