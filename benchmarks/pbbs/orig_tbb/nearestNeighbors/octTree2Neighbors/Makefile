include parallelDefs

# required files

NEAREST_NEIGHBORS = geometry.h nearestNeighbors.h blockRadixSort.h transpose.h quickSort.h
GLOBAL_REQUIRE = parallel.h sequence.h $(NEAREST_NEIGHBORS)
LOCAL_REQUIRE = octTree.h
INCLUDE = nearestNeighbors.h

PCFLAGS += -I$(TBBROOT)/include -std=c++11
PLFLAGS += -L$(TBBROOT)/obj -ltbb -std=c++11

include ../common/timeRequiredFiles
include MakeBenchI
