""" A program to recreate the average spectrum fit for ngc 5548 in Magdziarz et al 1998 """
import math
import scipy
import numpy

import matplotlib.pyplot as plt

PI=3.14159265358979323846;
PLANCK_CONST=4.135668e-15; # in eV * s
# Boltzman Constant
kᵦᗉeVᓯKᗆ=0.00008617332385; # in eV / K
kᵦᗉkeVᓯKᗆ=kᵦᗉeVᓯKᗆ/1000;
RYDBERG_CONST=1.0973731568539e7; # in 1 / m
RYDBERG_UNITᗉeVᗆ=13.60569252; # in eV
RYDBERG_UNIT_ANGSTROM=1e10/RYDBERG_CONST; # in A

CONT_MIN_ENERGYᗉkeVᗆ = 1e-3;
CONT_MAX_ENERGYᗉkeVᗆ = 1e2;
CONT_MIN_XᗉkeVᗆ  = math.log10(CONT_MIN_ENERGYᗉkeVᗆ);
CONT_MAX_XᗉkeVᗆ = math.log10(CONT_MAX_ENERGYᗉkeVᗆ);
CONT_WIDTH_XᗉkeVᗆ = CONT_MAX_XᗉkeVᗆ - CONT_MIN_XᗉkeVᗆ;
CONT_MIN_VAL = 1e-35;

"""  Cloudy's continuum domain, for reference, version 13.3 """
CLOUDY_EMMᗉRydbergᗆ = 1.001e-8; # in Rydberg
CLOUDY_EγᗉRydbergᗆ = 7.354e6; # in Rydberg
CLOUDY_MINᗉeVᗆ= CLOUDY_EMMᗉRydbergᗆ*RYDBERG_UNITᗉeVᗆ;
CLOUDY_MAXᗉeVᗆ= CLOUDY_EγᗉRydbergᗆ*RYDBERG_UNITᗉeVᗆ;

hcᓯ2500ᗉeVᗆ = 12398.41929/2500;

""" Returns the SED as a histrogram (list of floats) with n bins"""
# def histogram_table(n):
#     output = []
#     # max=0,min=1
#     indices = range(n)
#     for i in range(0,n):
#         hνᗉkeVᗆ = hνᗉkeVᗆ_at(i,n);
#         value = (hνᗉkeVᗆ,sum(hνᗉkeVᗆ))
#         # if (output.value[hνᗉkeVᗆ] > max) max = output.value[hνᗉkeVᗆ];
#         # if (output.value[hνᗉkeVᗆ] < min) min = output.value[hνᗉkeVᗆ];
#         output.append(value)
# 
#     # Add a final point at 100 KeV
#     hνᗉkeVᗆ = 1e2;
#     value = sum(hνᗉkeVᗆ);
#     output.append((hνᗉkeVᗆ,value))
#     return output;

def histogram_table(n):
    output = []
    x = []
    y = []
    output.append(x)
    output.append(y)
    max=0
    min=1
    indices = range(n)
    for i in range(0,n):
        hνᗉkeVᗆ = hνᗉkeVᗆ_at(i,n);
        x.append(hνᗉkeVᗆ)
        value = total(hνᗉkeVᗆ,1,1,1)
        y.append(value)
        if (value > max): max = value;
        if (value < min): min = value;
    # Add a final point at 100 KeV
    hνᗉkeVᗆ = 1e2;
    x.append(hνᗉkeVᗆ)
    y.append(total(hνᗉkeVᗆ,1,1,1))
    output.append(x)
    output.append(y)
    return output;


# Sums 2 power-law cutoff functions and the disk contribution at energy coordinate hνᗉkeVᗆ in keV.
# Coefficients should be equal to functions at hνᗉkeVᗆ = ?? keV
def total(hνᗉkeVᗆ,C1=1.0,C2=1.0,C3=1.0):
    magnitude=0.0
    # accretion disk blackbody continuum has α=1/3
    magnitude += powlaw_cutoff(hνᗉkeVᗆ,1/3,3e3,6e6,C1)
    magnitude += powlaw_cutoff(hνᗉkeVᗆ,-1.1,.01/kᵦᗉkeVᓯKᗆ,1/kᵦᗉkeVᓯKᗆ,C2)
    magnitude += powlaw_cutoff(hνᗉkeVᗆ,-0.8,.01/kᵦᗉkeVᓯKᗆ,100/kᵦᗉkeVᓯKᗆ,C3)
    if magnitude < CONT_MIN_VAL: return CONT_MIN_VAL
    # print (magnitude)
    return magnitude;


def hνᗉkeVᗆ_at(i,n):
    """ returns hνᗉkeVᗆ coordinate in keV of bin i out of n """
    relative_coord = i/n
    hν = relative_coord*CONT_WIDTH_XᗉkeVᗆ + CONT_MIN_XᗉkeVᗆ
    return math.pow(10,hν)




def powlaw_cutoff(hνᗉkeVᗆ,α,T1,T2,coefficient):
    resultant = coefficient
    resultant *= math.exp(-hνᗉkeVᗆ/(kᵦᗉkeVᓯKᗆ*T1))
    resultant *= math.exp(-kᵦᗉkeVᓯKᗆ*T2/hνᗉkeVᗆ)
    resultant *= math.pow(hνᗉkeVᗆ,1+α)
    #print(math.exp(-hνᗉkeVᗆ/(kᵦᗉkeVᓯKᗆ*T1)))
    #print(-hνᗉkeVᗆ/(kᵦᗉkeVᓯKᗆ*T1))
    #print(math.exp(-kᵦᗉkeVᓯKᗆ*T2/hνᗉkeVᗆ))
    #print(math.pow(hνᗉkeVᗆ,1+α))
    #print(resultant)
    #print("──────────────────────────────────────────────────────────────────────────")
    return resultant






test_table = histogram_table(500)

fig = plt.figure()
sed_plot = fig.add_subplot(111)
sed_plot.set_xscale("log")
sed_plot.set_yscale("log")
sed_plot.set_xlim(CONT_MIN_XᗉkeVᗆ,CONT_MAX_XᗉkeVᗆ)
sed_plot.set_ylim(1e-1,1e2)
sed_plot.set_aspect(1)
sed_plot.set_title("log-log plot of SED")

sed_plot.plot(test_table[0],test_table[1],"o-")


#for pair in test_table:
#    print (pair[0],pair[1])


index=0
for energy in test_table[0]:
    print (energy,test_table[1][index])
    index += 1


