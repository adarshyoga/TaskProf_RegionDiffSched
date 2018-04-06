include parallelDefs

# required files
GLOBAL_REQUIRE = utils.h parallel.h sequence.h deterministicHash.h
LOCAL_REQUIRE = deterministicHash.h 
INCLUDE = deterministicHash.h

PCFLAGS += -I$(TBBROOT)/include -std=c++11 -fpermissive
PLFLAGS += -L$(TBBROOT)/obj -ltbb -std=c++11

include ../common/timeRequiredFiles
include MakeBenchI
