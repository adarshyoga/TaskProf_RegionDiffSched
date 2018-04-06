CXX=g++

INCLUDE = -I$(TP_ROOT)/include -I$(TBBROOT)/include

TARGET=karatsuba
OBJS=karatsuba.o

CXXFLAGS := $(CXXFLAGS) -std=c++0x -g
LIBS := $(LIBS) -ltbb -ltprof

LDFLAGS = -L$(TBBROOT)/obj -L$(TP_ROOT)/obj

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(INCLUDE) $(LIBS) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TARGET) *~ *.out
