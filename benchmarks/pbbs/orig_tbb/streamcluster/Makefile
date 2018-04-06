# Makefile for streamcluster
CXX=g++

INCLUDE = -I$(TBBROOT)/include

TARGET=streamcluster
OBJS=streamcluster.o

CXXFLAGS := $(CXXFLAGS) -DTBB_VERSION
LIBS := $(LIBS) -ltbb

LDFLAGS = -L$(TBBROOT)/obj

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(INCLUDE) $(LIBS) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TARGET) *~ *.out
