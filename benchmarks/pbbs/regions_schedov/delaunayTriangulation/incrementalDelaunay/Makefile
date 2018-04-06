include parallelDefs

# required files
NEAREST_NEIGHBORS = geometry.h nearestNeighbors.h octTree.h blockRadixSort.h transpose.h
DELAUNAY = topology.h delaunayDefs.h delaunay.C 
GLOBAL_REQUIRE = parallel.h sequence.h $(NEAREST_NEIGHBORS) $(DELAUNAY)
LOCAL_REQUIRE = 
OBJS = delaunay.o 

PCFLAGS += -I$(TP_ROOT)/include -I$(TBBROOT)/include -std=c++11 -fpermissive
PLFLAGS += -L$(TBBROOT)/obj -ltbb -L$(TP_ROOT)/obj -ltprof -std=c++11

include ../common/timeRequiredFiles
include MakeBench
