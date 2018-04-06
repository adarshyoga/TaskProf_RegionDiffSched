CXX=g++

TARGET   = fluidanimate
OBJS     = tbb.cpp cellpool.o
LIBS	 += -ltbb
CXXFLAGS += -D_GNU_SOURCE -D__XOPEN_SOURCE=600
LDFLAGS = -L$(TBBROOT)/obj
INCLUDE += -I$(TBBROOT)/include

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
