include parallelDefs

# required files
GLOBAL_REQUIRE = parallel.h sequence.h geometry.h
LOCAL_REQUIRE = 
OBJS = hull.o
PCFLAGS += -I$(TP_ROOT)/include -I$(TBBROOT)/include -std=c++11 -fpermissive
PLFLAGS += -L$(TBBROOT)/obj -ltbb -L$(TP_ROOT)/obj -ltprof -std=c++11

include ../common/timeRequiredFiles
include MakeBench
