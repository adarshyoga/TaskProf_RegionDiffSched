CXX=g++

TARGET   = fluidanimate
OBJS     = tbb.cpp cellpool.o
LIBS	 += -ltbb -ltprof
CXXFLAGS += -D_GNU_SOURCE -D__XOPEN_SOURCE=600 -std=c++11
LDFLAGS = -L$(TBBROOT)/obj -L$(TP_ROOT)/obj
INCLUDE += -I$(TP_ROOT)/include -I$(TBBROOT)/include

all: tbb fluidcmp

tbb: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(INCLUDE) $(LIBS) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

fluidcmp: fluidcmp.cpp
	rm -rf fluidcmp
	$(CXX) $(CXXFLAGS) fluidcmp.cpp -o fluidcmp

clean:
	rm -rf $(TARGET)
	rm -rf fluidcmp *~ *.out *.o
