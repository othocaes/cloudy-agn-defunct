#!/bin/bash
mkdir -p ../bin/
source_name=`echo $1|sed 's/\.cpp$//'`
#g++ -c ${source_name}.cpp `libpng-config --cflags` -I shapelib-1.3.0/ -I png++-0.2.5/
g++ -c ${source_name}.cpp `libpng-config --cflags` -I png++-0.2.5/
g++ -o ../bin/${source_name} ${source_name}.o `libpng-config --ldflags` 

