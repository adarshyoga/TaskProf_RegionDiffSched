CXX=g++

INCLUDE = -I$(TBBROOT)/include

TARGET=kmeans
OBJS=test_kmeans.o kmeans_tbb.o repair_empty_clusters.o

CXXFLAGS := $(CXXFLAGS) -std=c++0x
LIBS := $(LIBS) -ltbb

LDFLAGS = -L$(TBBROOT)/obj

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(INCLUDE) $(LIBS) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TARGET) *~ *.out
