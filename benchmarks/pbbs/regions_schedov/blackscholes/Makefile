CXX=g++
CSRC    = $(SRC)
TARGET  = blackscholes
LDFLAGS = -L$(TBBROOT)/obj -L$(TP_ROOT)/obj
#LIBS    = -ltbb -ltbbmalloc_proxy -ltbbmalloc -ldl -lrt -lpthread
LIBS = -ltbb -ltprof
CXXFLAGS += -I$(TBBROOT)/include -I$(TP_ROOT)/include -g -std=c++11

SRC	= blackscholes.c
MT        = -DENABLE_TBB

# Default build single precision version
NCO     = -DNCO=4

CXXFLAGS += $(MT) $(NCO) $(FUNC) $(ERR) $(CSRC)

all: $(TARGET)

$(TARGET): clean $(CSRC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(M4_SRC) *~ *.out prices.txt
