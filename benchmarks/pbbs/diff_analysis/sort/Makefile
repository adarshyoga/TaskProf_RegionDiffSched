CXX=g++

INCLUDE = -I$(TP_ROOT)/include -I$(TBBROOT)/include

TARGET=sort
OBJS=test_sort.o

CXXFLAGS := $(CXXFLAGS) -std=c++0x
LIBS := $(LIBS) -ltbb -ltprof

LDFLAGS = -L$(TBBROOT)/obj -L$(TP_ROOT)/obj

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(INCLUDE) $(LIBS) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TARGET) *~ *.out
