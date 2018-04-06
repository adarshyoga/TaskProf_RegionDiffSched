include parallelDefs

# required files
GLOBAL_REQUIRE = utils.h parallel.h sequence.h deterministicHash.h
LOCAL_REQUIRE = 
INCLUDE = deterministicHash.h

PCFLAGS += -I$(TP_ROOT)/include -I$(TBBROOT)/include -std=c++11 -fpermissive
PLFLAGS += -L$(TBBROOT)/obj -ltbb -L$(TP_ROOT)/obj -ltprof -std=c++11

include ../common/timeRequiredFiles
include MakeBenchI
