include parallelDefs

# required files

NEAREST_NEIGHBORS = geometry.h nearestNeighbors.h blockRadixSort.h transpose.h quickSort.h
GLOBAL_REQUIRE = parallel.h sequence.h $(NEAREST_NEIGHBORS)
LOCAL_REQUIRE = octTree.h
INCLUDE = nearestNeighbors.h

PCFLAGS += -I$(TP_ROOT)/include -I$(TBBROOT)/include -std=c++11 -fpermissive
PLFLAGS += -L$(TBBROOT)/obj -ltbb -L$(TP_ROOT)/obj -ltprof -std=c++11

include ../common/timeRequiredFiles
include MakeBench
