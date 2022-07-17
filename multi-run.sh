#!/bin/bash

if [ "$1" == "" ]
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
            mpirun --oversubscribe -n "${world_size}" "$1" "${width}"
            echo "Sleeping..."
            sleep 5
        done
    done
done
