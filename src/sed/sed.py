
#parents, babies = (1, 1)
#while babies < 100:
#    #print 'This generation has {0} babies'.format(babies)
#    print('This generation has',babies,'babies.')
#    parents, babies = (babies, parents + babies)
import math 

PI=3.14159265358979323846;
PLANCK_CONST=4.135668e-15; # in eV * s
BOLTZMANN_CONST=0.00008617332385; # in eV / K
RYDBERG_CONST=1.0973731568539e7; # in 1 / m
RYDBERG_UNIT_EV=13.60569252; # in eV
RYDBERG_UNIT_ANGSTROM=1e10/RYDBERG_CONST; # in A

CONT_MIN_ENERGY = 1e-2; # eV
CONT_MAX_ENERGY = 1e5; # eV
CONT_MIN_X = math.log10(CONT_MIN_ENERGY);
CONT_MAX_X = math.log10(CONT_MAX_ENERGY);
CONT_WIDTH_X = CONT_MAX_X - CONT_MIN_X;
CONT_MIN_VAL = 1e-35;

"""  Cloudy's continuum domain, for reference, version 13.3 """
CLOUDY_EMM = 1.001e-8; # in Rydberg
CLOUDY_EGAMRY = 7.354e6; # in Rydberg
CLOUDY_MIN_EV = CLOUDY_EMM*RYDBERG_UNIT_EV;
CLOUDY_MAX_EV = CLOUDY_EGAMRY*RYDBERG_UNIT_EV;

IN_EV_2500A = 12398.41929/2500;




def hnu_at(i,n):
    """ returns hnu coordinate of bin i out of n """
    relative_coord = i/n
    x_coord = relative_coord*CONT_WIDTH_X + CONT_MIN_X;
    return math.pow(10,x_coord);

def histogram_table(n):
    output = []
    # max=0,min=1
    indices = range(n)
    for i in range(0,n):
        hnu = hnu_at(i,n);
        value = (hnu,sed(hnu))
        # if (output.value[hnu] > max) max = output.value[hnu];
        # if (output.value[hnu] < min) min = output.value[hnu];
        output.append(value)

    # Add a final point at 100 KeV
    hnu = 1e5;
    value = sed(hnu);
    output.append((hnu,value))
    return output;

def sed(hnu):
    magnitude=0.0;
    magnitude += test_pow_law(hnu)
    if magnitude < CONT_MIN_VAL: return CONT_MIN_VAL
        # magnitude = CONT_MIN_VAL;
    return magnitude;

def test_pow_law(hnu):
    low_cutoff,α,high_cutoff = (.1,1,5)
    resultant = 1
    resultant *= math.exp(-hnu/high_cutoff)
    resultant *= math.exp(-low_cutoff/hnu)
    resultant *= math.pow(hnu,1+α)
    return resultant

# for t in (22.6, 25.8, 27.3, 29.8):+P B
#     print(t, ": ", fahrenheit(t))


test_table = histogram_table(500)

for pair in test_table:
    print (pair[0],pair[1])