include parallelDefs

# required files
TOPOLOGY = geometry.h topology.h topologyFromTri.C 
HASH = deterministicHash.h
GLOBAL_REQUIRE = parallel.h sequence.h $(HASH) $(TOPOLOGY)
LOCAL_REQUIRE = 
OBJS = refine.o topologyFromTri.o

PCFLAGS += -I$(TP_ROOT)/include -I$(TBBROOT)/include -std=c++11 -fpermissive
PLFLAGS += -L$(TBBROOT)/obj -ltbb -L$(TP_ROOT)/obj -ltprof -std=c++11

include ../common/timeRequiredFiles
include MakeBench
