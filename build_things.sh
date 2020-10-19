#! /bin/bash

CDIR=$(pwd)
mkdir ../graphitBUILD
cd ../graphitBUILD
cmake $CDIR
make -j

# Make each algo
python3 bin/graphitc.py -f $CDIR/apps/sssp_delta_stepping.gt -o sssp.cpp
python3 bin/graphitc.py -f $CDIR/apps/sir.gt -o sir.cpp
python3 bin/graphitc.py -f $CDIR/apps/pi.gt -o pi.cpp
python3 bin/graphitc.py -f $CDIR/apps/pagrank_nd.gt -o pr_nd.cpp

# Compile
g++ --std=c++14 -O3 -DOPENMP -fopenmp -I ${CDIR}/src/runtime_lib/ sssp.cpp 	${CDIR}/apps/sir_helper_fn.cpp -o sssp.exe
g++ --std=c++14 -O3 -DOPENMP -fopenmp -I ${CDIR}/src/runtime_lib/ sir.cpp 	${CDIR}/apps/sir_helper_fn.cpp -o sir.exe
g++ --std=c++14 -O3 -DOPENMP -fopenmp -I ${CDIR}/src/runtime_lib/ pi.cpp 		-o pi.exe
g++ --std=c++14 -O3 -DOPENMP -fopenmp -I ${CDIR}/src/runtime_lib/ pr_nd.cpp -o pr.exe

cd $CDIR
mkdir binaries
cp ../graphitBUILD/pi.exe binaries/
cp ../graphitBUILD/pr.exe binaries/
cp ../graphitBUILD/sssp.exe binaries/
cp ../graphitBUILD/sir.exe binaries/
