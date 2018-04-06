#ifneq (,$(shell which clang++ 2>/dev/null))
#CXX=clang++
#CXXFLAGS = -I$(TD_ROOT)/include -I$(TBBROOT)/include -ftaskdebug -O0
#else
CXX=g++
CXXFLAGS = -I$(TBBROOT)/include -std=c++11
#endif # clang++

LIBS = -L$(TBBROOT)/obj -ltbb -lpthread
#LIBS = -ltbb -L$(TD_ROOT)/obj -ltdebug

SRCS = $(wildcard *.cpp)

PROGS = $(patsubst %.cpp,%,$(SRCS))

all: $(PROGS)

%: %.cpp
	$(CXX) -g $(CXXFLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf $(PROGS) *~ *.out
