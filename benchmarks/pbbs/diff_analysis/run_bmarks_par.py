#! /usr/bin/python

import sys, string, os, popen2, shutil, platform, subprocess, pprint, time
import util
from math import sqrt

#clean up the obj directories
do_clean = False

#build the configs
do_build = False

#run the benchmarks
do_run = True

#collect data to plot
do_collect_data = True

if do_clean and not do_build:
    print "Clean - true and build - false not allowed"
    exit

#set paths
TBBROOT=os.environ['TBBROOT']
print "TBBROOT = " + TBBROOT
TP_ROOT=os.environ['TP_ROOT']
print "TP_ROOT = " + TP_ROOT
TP_GENPROF=os.environ['TP_GENPROF']
print "TP_GENPROF = " + TP_GENPROF

configs = []

entry = { "NAME" : "RUN_ALL_BENCHMARKS",
          "NUM_RUNS" : 1,
          "CLEAN_LINE" : " make clean ",
          "BUILD_LINE" : " make ",
          "BUILD_ARCH" : "x86_64",
          "RUN_ARCH" : "x86_64",
          #"RUN_LINE" : "perf stat -e instructions:u,cycles:u,cache-misses:u,cache-references:u ./",
          #"RUN_LINE" : "numactl --cpunodebind=0 ./",
          "RUN_LINE" : "./",
          "ARGS" : "",
          }

configs.append(entry);

ref_cwd = os.getcwd();
arch = platform.machine()
full_hostname = platform.node()
hostname=full_hostname

benchmarks=[
    "blackscholes",
    "fluidanimate",
    "streamcluster",
    "swaptions",
    "convexHull",
    "delaunayRefine",
    "delaunayTriangulation",
    "karatsuba",
    "kmeans",
    "nearestNeighbors",
    "rayCast",
    "sort",
    "comparisonSort",
    "integerSort",
    "removeDuplicates",
    "dictionary",
    "suffixArray",
    "breadthFirstSearch",
    "maximalIndependentSet",
    "maximalMatching",
    "minSpanningForest",
    "spanningForest",
    "nBody",
]

inner_folder=[
    ".",
    ".",
    ".",
    ".",
    "quickHull",
    "incrementalRefine",
    "incrementalDelaunay",
    ".",
    ".",
    "octTree2Neighbors",
    "kdTree",
    ".",
    "sampleSort",
    "blockRadixSort",
    "deterministicHash",
    "deterministicHash",
    "parallelKS",
    "deterministicBFS",
    "incrementalMIS",
    "incrementalMatching",
    "parallelKruskal",
    "incrementalST",
    "parallelCK",
]

executable=[
    "blackscholes",
    "fluidanimate",
    "streamcluster",
    "swaptions",
    "hull",
    "refine",
    "delaunay",
    "karatsuba",
    "kmeans",
    "neighbors",
    "ray",
    "sort",
    "sort",
    "isort",
    "remDups",
    "dict",
    "SA",
    "BFS",
    "MIS",
    "matching",
    "MST",
    "ST",
    "nbody",
]

inputs_ser=[
    "1 in_10M.txt prices.txt",
    "1 500 in_500K.fluid out.fluid",
    "10 20 128 1000000 200000 5000 none output.txt 1",
    "-ns 64 -sm 40000 -nt 1",
    "-r 1 -o /tmp/ofile971367_438110 -t 1 ../geometryData/data/2DinSphere_10M",
    "-r 1 -o /tmp/ofile699250_954868 -t 1 ../geometryData/data/2DinCubeDelaunay_2000000",
    "-r 1 -o /tmp/ofile850740_480180 -t 1 ../geometryData/data/2DinCube_10M",
    "1",
    "1",
    "-d 2 -k 1 -r 1 -o /tmp/ofile677729_89710 -t 1 ../geometryData/data/2DinCube_10M",
    "-r 1 -o /tmp/ofile136986_843068 -t 1 ../geometryData/data/happyTriangles ../geometryData/data/happyRays",
    "1",
    "-r 1 -o /tmp/ofile959994_651631 -t 1 ../sequenceData/data/exptSeq_10M_double",
    "-r 1 -o /tmp/ofile660322_977147 -t 1 ../sequenceData/data/randomSeq_10M_256_int_pair_int",
    "-r 1 -o /tmp/ofile530965_884365 -t 1 ../sequenceData/data/trigramSeq_10M_pair_int",
    "-r 1 -o /tmp/ofile765299_409340 -t 1 ../sequenceData/data/trigramSeq_10M_pair_int",
    "-r 1 -o /tmp/ofile802103_686914 -t 1 ../sequenceData/data/trigramString_10000000",
    "-r 1 -o /tmp/ofile564664_442514 -t 1 ../graphData/data/randLocalGraph_J_5_10000000",
    "-r 1 -o /tmp/ofile470293_748866 -t 1 ../graphData/data/randLocalGraph_J_5_10000000",
    "-r 1 -o /tmp/ofile685095_551810 -t 1 ../graphData/data/rMatGraph_E_5_10000000",
    "-r 1 -o /tmp/ofile897171_477798 -t 1 ../graphData/data/rMatGraph_WE_5_10000000",
    "-r 1 -o /tmp/ofile780084_677212 -t 1 ../graphData/data/randLocalGraph_E_5_10000000",
    "-r 1 -o /tmp/ofile974877_207802 -t 1 ../geometryData/data/3DonSphere_1000000",
]

inputs_par=[
    "16 in_10M.txt prices.txt",
    "16 500 in_500K.fluid out.fluid",
    "10 20 128 1000000 200000 5000 none output.txt 16",
    "-ns 64 -sm 40000 -nt 16",
    "-r 1 -o /tmp/ofile971367_438110 -t 16 ../geometryData/data/2DinSphere_10M",
    "-r 1 -o /tmp/ofile699250_954868 -t 16 ../geometryData/data/2DinCubeDelaunay_2000000",
    "-r 1 -o /tmp/ofile850740_480180 -t 16 ../geometryData/data/2DinCube_10M",
    "16",
    "16",
    "-d 2 -k 1 -r 1 -o /tmp/ofile677729_89710 -t 16 ../geometryData/data/2DinCube_10M",
    "-r 1 -o /tmp/ofile136986_843068 -t 16 ../geometryData/data/happyTriangles ../geometryData/data/happyRays",
    "16",
    "-r 1 -o /tmp/ofile959994_651631 -t 16 ../sequenceData/data/exptSeq_10M_double",
    "-r 1 -o /tmp/ofile660322_977147 -t 16 ../sequenceData/data/randomSeq_10M_256_int_pair_int",
    "-r 1 -o /tmp/ofile530965_884365 -t 16 ../sequenceData/data/trigramSeq_10M_pair_int",
    "-r 1 -o /tmp/ofile765299_409340 -t 16 ../sequenceData/data/trigramSeq_10M_pair_int",
    "-r 1 -o /tmp/ofile802103_686914 -t 16 ../sequenceData/data/trigramString_10000000",
    "-r 1 -o /tmp/ofile564664_442514 -t 16 ../graphData/data/randLocalGraph_J_5_10000000",
    "-r 1 -o /tmp/ofile470293_748866 -t 16 ../graphData/data/randLocalGraph_J_5_10000000",
    "-r 1 -o /tmp/ofile685095_551810 -t 16 ../graphData/data/rMatGraph_E_5_10000000",
    "-r 1 -o /tmp/ofile897171_477798 -t 16 ../graphData/data/rMatGraph_WE_5_10000000",
    "-r 1 -o /tmp/ofile780084_677212 -t 16 ../graphData/data/randLocalGraph_E_5_10000000",
    "-r 1 -o /tmp/ofile974877_207802 -t 16 ../geometryData/data/3DonSphere_1000000",
]

BTRACK_CONFIG = "./configure --enable-tbb --disable-threads --disable-openmp --prefix=" + ref_cwd + "/bodytrack CXXFLAGS=\"-O3 -funroll-loops -fprefetch-loop-arrays -fpermissive -fno-exceptions -static-libgcc -Wl,--hash-style=both,--as-needed -DPARSEC_VERSION=3.0-beta-20150206 -fexceptions -I" + TBBROOT + "/include -I" + TP_ROOT + "/include\" LDFLAGS=\"-L" + TBBROOT + "\obj -L" + TP_ROOT + "/obj\" LIBS=\"-ltbb -ltprof\" VPATH=\".\""

f = open('runtimes_prof_ser_FINAL.txt', 'w')

for config in configs:
    util.log_heading(config["NAME"], character="-")
    if do_clean:
        util.chdir(TP_ROOT)
        util.run_command("make clean", verbose=False)
        util.chdir(TBBROOT)
        util.run_command("make clean", verbose=False)
    if do_build:
        util.chdir(TP_ROOT)
        util.run_command("make", verbose=False)
        util.chdir(TBBROOT)
        util.run_command("make", verbose=False)
    #util.chdir(PARSEC_ROOT)

    for benchmark in benchmarks:
        util.chdir(ref_cwd)
        runtimes = []
        for i in range(0, config["NUM_RUNS"]):
            try:
                util.chdir(ref_cwd + "/" + benchmark + "/" + inner_folder[benchmarks.index(benchmark)])
                util.log_heading(benchmark, character="=")
                if do_run:
                    try:
                        clean_string = config["CLEAN_LINE"]
                        util.run_command(clean_string, verbose=False)
                    except:
                        print "Clean failed"
                    if benchmark == "bodytrack":
                        try:
                            util.run_command("make distclean", verbose=False)
                        except:
                            print "Dist clean failed"

                    if benchmark == "bodytrack":
                        util.run_command(BTRACK_CONFIG, verbose=False)

                    build_string = config["BUILD_LINE"]
                    util.run_command(build_string, verbose=False)

                    run_string = config["RUN_LINE"] + executable[benchmarks.index(benchmark)] + " " + inputs_ser[benchmarks.index(benchmark)]
                    if benchmark == "bodytrack":
                        util.chdir(ref_cwd + "/" + benchmark + "/" + "TrackingBenchmark")
                    start = time.time()
                    util.run_command(run_string, verbose=False)
                    elapsed = (time.time() - start)
                    runtimes.append(elapsed)

                    util.run_command("mv *.csv step_nodes_serial/", verbose=False)
                    
                    #for tp in range(0,5):
                    run_string = config["RUN_LINE"] + executable[benchmarks.index(benchmark)] + " " + inputs_par[benchmarks.index(benchmark)]
                    if benchmark == "bodytrack":
                        util.chdir(ref_cwd + "/" + benchmark + "/" + "TrackingBenchmark")
                    util.run_command(run_string, verbose=False)
                    
                    gen_prof_string = TP_GENPROF + "/gentprof"
                    util.run_command(gen_prof_string, verbose=False)
                    #util.run_command("cp ws_profile.csv ws_profile.csv." + str(tp) + "thds", verbose=False)
                    
            except util.ExperimentError, e:
                print "Error: %s" % e
                print "-----------"
                print "%s" % e.output
                continue

        #if not len(orig_runtimes) == 0 and not len(runtimes) == 0:
        if not len(runtimes) == 0:
            #rt_str = benchmark + ":" + base_runtimes[benchmarks.index(benchmark)] + ":" + str(float(float(sum(runtimes))/float(len(runtimes))))
            #rt_str = benchmark + ":" + str(float(float(sum(orig_runtimes))/float(len(orig_runtimes)))) + ":" + str(float(float(sum(runtimes))/float(len(runtimes))))
            rt_str = benchmark + ":" + str(float(float(sum(runtimes))/float(len(runtimes))))
            print rt_str
            print ""
            f.write(rt_str + "\n")
f.close()
util.chdir(ref_cwd)
