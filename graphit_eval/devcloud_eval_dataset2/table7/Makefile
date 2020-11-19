COMMON_FLAGS = -std=gnu++1y -g -I ../../../src/runtime_lib/
CILK_FLAGS = $(COMMON_FLAGS)
OPENMP_FLAGS = $(COMMON_FLAGS)
TBB_FLAGS = $(COMMON_FLAGS)
GRAPHITC_PY = ../../../build/bin/graphitc.py
GRAPHIT_APP_DIR = ./apps
GRAPHIT_SCHEDULE_DIR= ./schedules

ifdef GCC_PAR
# compiling with gcc CILK and OPENMP
PCC = g++
CILK_FLAGS +=  -fcilkplus -lcilkrts -O3 -DCILK
OPENMP_FLAGS += -DOPENMP -fopenmp -O3 

else ifdef ICPC_PAR
# compiling with Intel icpc compiler
PCC = icpc
CILK_FLAGS += -qopenmp -DCILK -O3
OPENMP_FLAGS += -DOPENMP -qopenmp -O3

else ifdef ICPC_TBB
# compiling with Intel icpc compiler
PCC = icpc
TBB_FLAGS += -qopenmp -DTBB -O3 -ltbb
OPENMP_FLAGS += -DOPENMP -qopenmp -O3

else ifdef TBB 
PCC = g++
TBB_FLAGS += -I /data/scratch/yunming/intel_parallel_studio_cluster/parallel_studio_xe_2017/install/compilers_and_libraries_2017.2.174/linux/tbb/include/ -L /data/scratch/yunming/intel_parallel_studio_cluster/parallel_studio_xe_2017/install/compilers_and_libraries_2017.2.174/linux/tbb/lib/intel64/gcc4.7/ -fopenmp -DTBB -O3 -ltbb 
OPENMP_FLAGS += -DOPENMP -qopenmp -O3

else
# compiling serial programs
PCC = g++
CILK_FLAGS += -O3
OPENMP_FLAGS += -O3

endif

#the cpps files that are faster compiled with cilk 
pr_cilk_cpps = pagerank_pull pagerank_pull_segment 
sssp_cilk_cpps = sssp_hybrid_denseforward 

all: 
	make graphit_files
	make cpps

graphit_files:
	cp ../../../apps/pi.gt apps/pi.gt
	cp ../../../apps/sir.gt apps/sir.gt
	cp ../../../apps/sir_helper_fn.cpp apps/sir_helper_fn.cpp

	python3 ${GRAPHITC_PY} -a ${GRAPHIT_APP_DIR}/sssp.gt -f ${GRAPHIT_SCHEDULE_DIR}/sssp_hybrid_denseforward_parallel_cas.gt -o cpps/sssp_hybrid_denseforward.cpp
	python3 ${GRAPHITC_PY} -a ${GRAPHIT_APP_DIR}/sssp.gt -f ${GRAPHIT_SCHEDULE_DIR}/sssp_push_parallel_sliding_queue.gt -o cpps/sssp_push_slq.cpp

	python3 ${GRAPHITC_PY} -a ${GRAPHIT_APP_DIR}/sssp_delta_stepping.gt -f ${GRAPHIT_SCHEDULE_DIR}/priority_update_eager_with_merge_argv3.gt -o cpps/sssp_delta_stepping_with_merge.cpp
	python3 ${GRAPHITC_PY} -a ${GRAPHIT_APP_DIR}/sssp_delta_stepping.gt -f ${GRAPHIT_SCHEDULE_DIR}/priority_update_eager_no_merge_argv3.gt -o cpps/sssp_delta_stepping_no_merge.cpp

	python3 ${GRAPHITC_PY} -a ${GRAPHIT_APP_DIR}/pagerank.gt -f ${GRAPHIT_SCHEDULE_DIR}/pagerank_benchmark_pull_parallel.gt -o cpps/pagerank_pull.cpp
	python3 ${GRAPHITC_PY} -a ${GRAPHIT_APP_DIR}/pagerank.gt -f ${GRAPHIT_SCHEDULE_DIR}/pagerank_benchmark_cache.gt -o cpps/pagerank_pull_segment.cpp

	python3 ${GRAPHITC_PY} -f ${GRAPHIT_APP_DIR}/pi.gt  -o cpps/pi.cpp
	python3 ${GRAPHITC_PY} -f ${GRAPHIT_APP_DIR}/sir.gt  -o cpps/sir.cpp

cpps: openmp_only_cpps 
	# $(sssp_cilk_cpps) $(pr_cilk_cpps)

# this two cpps files can only be compiled with openmp to get parallel performance
openmp_only_cpps:
	$(PCC) $(OPENMP_FLAGS) -o bin/sssp_push_slq cpps/sssp_push_slq.cpp
	$(PCC) $(OPENMP_FLAGS) -o bin/sssp_delta_stepping_with_merge cpps/sssp_delta_stepping_with_merge.cpp
	$(PCC) $(OPENMP_FLAGS) -o bin/sssp_delta_stepping_no_merge cpps/sssp_delta_stepping_no_merge.cpp
	$(PCC) $(OPENMP_FLAGS) -o bin/pi cpps/pi.cpp ${GRAPHIT_APP_DIR}/sir_helper_fn.cpp
	$(PCC) $(OPENMP_FLAGS) -o bin/sir cpps/sir.cpp ${GRAPHIT_APP_DIR}/sir_helper_fn.cpp

% : cpps/%.cpp
	$(PCC) -o bin/$@ $< $(CILK_FLAGS) $(TBB_FLAGS)

clean:
	rm cpps/*.cpp
	rm bin/*
