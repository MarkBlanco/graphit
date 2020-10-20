#! /bin/bash

# Get DATADIR, SOURCESDIR, and GKCBASE, NUM_THREADS
source ./setup.sh

EXECS="sssp_delta_stepping.x" 
ALGO="SSSP"
OUTDIR="${GKCBASE}/${ALGO}_outputs/"
mkdir ${OUTDIR}

for EXEC in ${EXECS} 
do
	echo ${EXEC}
	# for symmetric graphs
	for GRAPH in road kron urand web twitter
	do
		name=${GRAPH}
		echo $name
		OUTPUT="${OUTDIR}${GRAPH}_${EXEC}.dat"
		echo $DATE >> ${OUTPUT}
		hostname   >> ${OUTPUT}
		echo ${EXEC} >> ${OUTPUT}
		CMD="./${EXEC} \
			${DATADIR}${name}.wsg < ${SOURCESDIR}${GRAPH}.sources" 
		echo $CMD
		./${EXEC} \
		  "${DATADIR}${name}.wsg"  "${SOURCESDIR}${GRAPH}.sources" >> ${OUTPUT} 2>&1
		awk -v lines=1 '/elapsed time/ {for(i=lines;i;--i)getline; t+=$0; num+=1 }\                                         
                       END  {if (num >0) print "average=", t/num}' ${OUTPUT} >> ${OUTPUT}
	done

	# # For non-symmetric graphs
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
	# 		${DATADIR}${name}.gr < ${SOURCESDIR}${GRAPH}.sources" 
	# 	echo $CMD
	# 	./${EXEC} \
	# 		"-t=${NUM_THREADS}" \
	# 		"${DATADIR}${name}.gr" < "${SOURCESDIR}${GRAPH}.sources" >> ${OUTPUT} 2>&1
	# done
done
