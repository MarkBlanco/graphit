#! /bin/bash

# Get DATADIR, SOURCESDIR, and GKCBASE, NUM_THREADS
source ./setup.sh

EXECS="pi.x" 
ALGO="ProteinInteraction"
OUTDIR="${GKCBASE}/${ALGO}_outputs/"
mkdir -p ${OUTDIR}

export OMP_NUM_THREADS=${NUM_THREADS}

for EXEC in ${EXECS} 
do
	echo ${EXEC}
	# for symmetric graphs
	for GRAPH in road #kron urand twitter web
	do
		name=${GRAPH}
		echo $name
		OUTPUT="${OUTDIR}${GRAPH}_${EXEC}.dat"
		echo $DATE >> ${OUTPUT}
		hostname   >> ${OUTPUT}
		echo ${EXEC} >> ${OUTPUT}
		CMD="./${EXEC}\
			${DATADIR}${name}.wsg" 
		echo $CMD
		./${EXEC} \
			"${DATADIR}${name}.wsg" >> ${OUTPUT} 2>&1
	done

	# For non-symmetric graphs
	# for GRAPH in web twitter
	# do
	# 	name=${GRAPH}
	# 	echo $name
	# 	OUTPUT="${OUTDIR}${GRAPH}_${EXEC}.dat"
	# 	echo $DATE >> ${OUTPUT}
	# 	hostname   >> ${OUTPUT}
	# 	echo ${EXEC} >> ${OUTPUT}
	# 	CMD="./${EXEC} \
	# 		-t=${NUM_THREADS} \
	# 		${DATADIR}${name}.gr" 
	# 	echo $CMD
	# 	./${EXEC} \
	# 		"-t=${NUM_THREADS}" \
	# 		"${DATADIR}${name}.gr" >> ${OUTPUT} 2>&1
	# done
done
