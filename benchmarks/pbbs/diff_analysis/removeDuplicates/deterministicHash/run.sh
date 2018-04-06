#!/bin/bash

echo "***** Building Remove Duplicates *****"
make

echo "***** Running serial version *****"
./remDups -r 1 -o /tmp/ofile530965_884365 -t 1 ../sequenceData/data/trigramSeq_10M_pair_int

echo "***** Moving serial data to step_nodes_serial directory *****"
if [ -d "step_nodes_serial" ]; then
    mv *.csv step_nodes_serial/
else
    mkdir step_nodes_serial
    mv *.csv step_nodes_serial/
fi

echo "***** Running parallel version *****"
./remDups -r 1 -o /tmp/ofile530965_884365 -t 16 ../sequenceData/data/trigramSeq_10M_pair_int

echo "***** Generating parallelism profile and differential profile *****"
${TP_GENPROF}/gentprof

echo "Completed! Check \"ws_profile.csv\" for parallelism profile. Check \"step_diff_file.csv\" for differential profile."
