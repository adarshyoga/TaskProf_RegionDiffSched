include parallelDefs

# required files
GLOBAL_REQUIRE = parallel.h sequence.h geometry.h
LOCAL_REQUIRE = 
OBJS = hull.o
PCFLAGS += -I$(TP_ROOT)/include -I$(TBBROOT)/include -std=c++11 -fpermissive
#PLFLAGS += -L/home/adarsh/apl-svn-repo/apl-projects/task_debug/tbb43_20141204oss/build/linux_intel64_gcc_cc5.4.0_libc2.23_kernel4.4.0_debug -ltbb_debug -L$(TP_ROOT)/obj -ltprof -std=c++11
PLFLAGS += -L$(TBBROOT)/obj -ltbb -L$(TP_ROOT)/obj -ltprof -std=c++11
#PLFLAGS += -L/home/adarsh/apl-svn-repo/apl-projects/task_debug/tbb43_20141204oss/build/linux_intel64_gcc_cc5.4.0_libc2.23_kernel4.4.0_release -ltbb -L$(TP_ROOT)/obj -ltprof -std=c++11

include ../common/timeRequiredFiles
include MakeBench
