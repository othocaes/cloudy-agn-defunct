""" A program to recreate the average spectrum fit for ngc 5548 in Magdziarz et al 1998 """
import math 
import scipy
import numpy

PI=3.14159265358979323846;
PLANCK_CONST=4.135668e-15; # in eV * s
BOLTZMANN_CONST=0.00008617332385; # in eV / K
RYDBERG_CONST=1.0973731568539e7; # in 1 / m
RYDBERG_UNIT_EV=13.60569252; # in eV
RYDBERG_UNIT_ANGSTROM=1e10/RYDBERG_CONST; # in A

CONT_MIN_ENERGY_keV = 1e-3;
CONT_MAX_ENERGY_keV = 1e2;
CONT_MIN_X = math.log10(CONT_MIN_ENERGY_keV);
CONT_MAX_X = math.log10(CONT_MAX_ENERGY_keV);
CONT_WIDTH_X = CONT_MAX_X - CONT_MIN_X;
CONT_MIN_VAL = 1e-35;

"""  Cloudy's continuum domain, for reference, version 13.3 """
CLOUDY_EMM = 1.001e-8; # in Rydberg
CLOUDY_EGAMRY = 7.354e6; # in Rydberg
CLOUDY_MIN_EV = CLOUDY_EMM*RYDBERG_UNIT_EV;
CLOUDY_MAX_EV = CLOUDY_EGAMRY*RYDBERG_UNIT_EV;

IN_EV_2500A = 12398.41929/2500;







""" Curve Parameters from MNRAS 301 Mdagziarz 1998 """


α_HC = 0.86


# Soft Excess
α_SE_sec2_1 = 1.1  # Quoted consistent with Korista 1995 and Marshall 1997
kT_SE_sec2_1 = .56 


# Comtonization fitted to ROSAT data
# ξ = 

# OSSE data fit
α_HC = 0.86
R = 0.96
E_cutoff_HC = .120 # keV, phase 1
F_HC = .38 # keV cm⁻² s⁻¹
# E_cutoff_HC = 118 # keV, phase 3
# F_HC = .61 # keV cm⁻² s⁻¹


# Section 3.3 values
kT_SE_sec3_2 = .270 # keV
α_SE_sec3_2 = 1.13
kT_HC_sec3_2 = 55 # keV
α_HC_sec3_2 = .76



def hν_at(i,n):
    """ returns hν coordinate of bin i out of n """
    relative_coord = i/n
    x_coord = relative_coord*CONT_WIDTH_X + CONT_MIN_X;
    return math.pow(10,x_coord);

def histogram_table(n):
    output = []
    # max=0,min=1
    indices = range(n)
    for i in range(0,n):
        hν = hν_at(i,n);
        value = (hν,sed(hν))
        # if (output.value[hν] > max) max = output.value[hν];
        # if (output.value[hν] < min) min = output.value[hν];
        output.append(value)

    # Add a final point at 100 KeV
    hν = 1e2;
    value = sed(hν);
    output.append((hν,value))
    return output;

def sed(hν):
    magnitude=0.0;
    magnitude += powlaw_cutoff(hν,α_HC,E_cutoff_HC,1) # OSSE data fit
    # magnitude += powlaw_cutoff(hν,α_SE_sec2_1,kT_SE_sec2_1,1)
    # magnitude += powlaw_cutoff(hν,α_SE_sec3_2,kT_SE_sec3_2,1)
    #magnitude += compt_approx(hν,-1.3,.345,.0034,1)
    if magnitude < CONT_MIN_VAL: return CONT_MIN_VAL
        # magnitude = CONT_MIN_VAL;
    return magnitude;

def powlaw_cutoff(hν,α,E_cutoff,norm):
    low_cutoff = .1
    resultant = norm
    resultant *= math.exp(-hν/E_cutoff)
    #resultant *= math.exp(-low_cutoff/hν)
    resultant *= math.pow(hν,1+α)
    return resultant

def compt_approx(hν,α,kT_keV,cutoff_keV,norm):
    magnitude =  math.pow(hν,(1+α))
    magnitude *= math.exp(-(hν/kT_keV))
    magnitude *= math.exp(-(cutoff_keV/hν))
    magnitude *= norm
    return magnitude

test_table = histogram_table(500)

for pair in test_table:
    print (pair[0],pair[1])