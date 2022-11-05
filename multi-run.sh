#!/bin/bash

RUN_MPI=0
RUN_OPENMP=0
case "$1" in
    "mpi")
        RUN_MPI=1
        ;;
    "openmp")
        RUN_OPENMP=1
        ;;
    *)
        echo "Unknown command"
        exit 1
        ;;
esac

if [ "$2" == "" ]
then
    echo "Usage: $0 <path to program>"
    exit 1
fi

set -o errexit
set -o nounset
set -o pipefail

readonly N_REPEATS="3"

for world_size in {1..16}
do
    for width in {100..5100..1000}
    do
        for repeat in $(seq 1 "${N_REPEATS}")
        do
            echo "world_size = ${world_size}; width = ${width}; repeat = ${repeat}"
            if [ ${RUN_MPI} -eq 1 ]; then
                mpirun --oversubscribe -n "${world_size}" "$2" "${width}"
            elif [ ${RUN_OPENMP} -eq 1 ]; then
                "${2}" ${world_size} ${width}
            fi
        done
    done
done
