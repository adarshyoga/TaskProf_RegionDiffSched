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

configs = []

entry = { "NAME" : "RUN_ALL_BENCHMARKS",
          "NUM_RUNS" : 3,
          "CLEAN_LINE" : " make clean ",
          "BUILD_LINE" : " make ",
          "BUILD_ARCH" : "x86_64",
          "RUN_ARCH" : "x86_64",
          #"RUN_LINE" : "perf stat -e instructions:u,cycles:u ./",
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
    "convexHull_opt",
    "delaunayRefine_opt",
    "delaunayTriangulation_opt",
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

BTRACK_CONFIG = "./configure --enable-tbb --disable-threads --disable-openmp --prefix=" + ref_cwd + "/bodytrack CXXFLAGS=\"-O3 -funroll-loops -fprefetch-loop-arrays -fpermissive -fno-exceptions -static-libgcc -Wl,--hash-style=both,--as-needed -DPARSEC_VERSION=3.0-beta-20150206 -fexceptions -I" + TBBROOT + "/include\" LDFLAGS=\"-L" + TBBROOT + "\obj\" LIBS=\"-ltbb\" VPATH=\".\""

f = open('runtimes.txt', 'w')

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

    for benchmark in benchmarks:
        util.chdir(ref_cwd)

        try:
            util.chdir(ref_cwd + "/" + benchmark + "/" + inner_folder[benchmarks.index(benchmark)])
            util.log_heading(benchmark, character="=")
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
            
            ser_runtimes = []
            for i in range(0, config["NUM_RUNS"]):
                run_string = config["RUN_LINE"] + executable[benchmarks.index(benchmark)] + " " + inputs_ser[benchmarks.index(benchmark)]
                if benchmark == "bodytrack":
                    util.chdir(ref_cwd + "/" + benchmark + "/" + "TrackingBenchmark")
                start = time.time()
                util.run_command(run_string, verbose=True)
                elapsed = (time.time() - start)
                ser_runtimes.append(elapsed)

            par_runtimes = []
            for i in range(0, config["NUM_RUNS"]):
                run_string = config["RUN_LINE"] + executable[benchmarks.index(benchmark)] + " " + inputs_par[benchmarks.index(benchmark)]
                if benchmark == "bodytrack":
                    util.chdir(ref_cwd + "/" + benchmark + "/" + "TrackingBenchmark")
                start = time.time()
                util.run_command(run_string, verbose=True)
                elapsed = (time.time() - start)
                par_runtimes.append(elapsed)
                
            rt_str = benchmark + ":" + str(float(float(sum(ser_runtimes))/float(len(ser_runtimes)))) + ":" + str(float(float(sum(par_runtimes))/float(len(par_runtimes))))
            #rt_str = benchmark + ":" + str(float(float(sum(runtimes))/float(len(runtimes))))
            print rt_str
            print ""
            f.write(rt_str + "\n")
                
        except util.ExperimentError, e:
            print "Error: %s" % e
            print "-----------"
            print "%s" % e.output
            continue

f.close()
util.chdir(ref_cwd)

# def geometric_mean(nums):
#     return (reduce(lambda x, y: x*y, nums))**(1.0/len(nums))

# benchmarks= []
# hash_table3_results = []
# labels = []

# i=0
# f=open("slowdown_data.txt",'r')

# total = 0.0
# total1 = 0.0
# total2 = 0.0

# for line in f:
#     #print line,
#     (bench, base, cur)= string.split(line, ':')
#     benchmarks.append(bench)
    
#     sdown = float(cur)/float(base)
#     hash_table3_results.append(float(sdown))
   
#     i += 1
   
# benchmarks.append("geomean")
# avg = geometric_mean(hash_table3_results)
# hash_table3_results.append(avg)

# i += 1

# #print i

# def generate_bar_example():
#    stacks=[]
#    bars=[]
#    tempval = 0.5
#    output_list = ""
#    for j in range(i):
#       bars=[]

#       numbers = []
#       if(float(hash_table3_results[j]) > 10):
#          if not (int(round(hash_table3_results[j])) == 10):
#             output_list = output_list + "graph 2 newstring fontsize 9 x " + str(tempval) + " y 102 hjc vjt rotate 90.0 : " + str(int(round(hash_table3_results[j]))) + "X" + "\n"
#             print int(round(hash_table3_results[j])), output_list
#          numbers.append(10)
#       else:
#          numbers.append(hash_table3_results[j])

#       numbers=mfgraph.stack_bars(numbers)
#       bars.append([""] + numbers)

#       stacks.append([benchmarks[j]]+ bars)
#       tempval += 2.15

#    #print stacks

#    return [mfgraph.stacked_bar_graph(stacks,
#                                      bar_segment_labels = labels,
#                                      #title = "Slowdown over base execution of all benchmarks",                                                                                                             
#                                      ylabel = "Slowdown",
#                                      #xlabel = "Number of pointer jumps",                                                                                                                                   
#                                      colors = ["0.375 0.375 0.375", "0.875 0.875 0.875", "0 0 0", "0.625 0.625 0.625"],
#                                      legend_x = "2",
#                                      legend_y = "80",
#                                      legend_type = "Manual",
#                                      legend_type_x=[0, 0, 18, 18],
#                                      legend_type_y=[15, 0, 15, 0] ,
#                                      #ysize = 1.1,
#                                      #xsize = 4,                                                                                                                                                            
#                                      ymax = 10,
#                                      patterns = ["solid"],
#                                      stack_name_rotate = 25.0,
#                                      stack_name_font_size = "9", #bmarks names                                                                                                                              
#                                      label_fontsize = "9", #y-axis name                                                                                                                                     
#                                      legend_fontsize = "9", #label names                                                                                                                                    
#                                      yhash_marks = [0, 5, 10],
#                                      yhash_names = ["0X", "5X", "10X"],
#                                      ) + output_list]

# mfgraph.run_jgraph("newpage\n".join(generate_bar_example()), "Slowdown_graph")
