CXX=g++

INCLUDE = -I$(TBBROOT)/include

TARGET=sort
OBJS=test_sort.o

CXXFLAGS := $(CXXFLAGS) -std=c++0x
LIBS := $(LIBS) -ltbb

LDFLAGS = -L$(TBBROOT)/obj

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(INCLUDE) $(LIBS) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TARGET) *~ *.out
