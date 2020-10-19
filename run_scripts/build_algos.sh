#!/bin/bash

cd /home/upasana/graphit/build/bin

for alg in pi pagerank
do
    python3 graphitc.py -f ../../apps/${alg}.gt -o ${alg}.cpp
    g++ -I ../../src/runtime_lib/ -std=c++14 -O3 -DOPENMP -fopenmp ${alg}.cpp -o /home/upasana/graphit/run_scripts/${alg}.x
done

for alg in sssp_delta_stepping sir
do
    python3 graphitc.py -f ../../apps/${alg}.gt -o ${alg}.cpp && g++ -I ../../src/runtime_lib/ -std=c++14 -O3 -DOPENMP -fopenmp ${alg}.cpp ../../apps/sir_helper_fn.cpp  -o /home/upasana/graphit/run_scripts/${alg}.x
done
