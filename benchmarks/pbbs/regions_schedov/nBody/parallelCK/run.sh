#!/bin/bash

make

#First run the program to generate profile data
./nbody -r 1 -o /tmp/ofile974877_207802 ../geometryData/data/3DonSphere_1000000

#generate TaskProf profile from the profile data
$TP_GENPROF/gentprof
