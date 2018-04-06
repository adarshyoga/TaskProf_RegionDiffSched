CXX=g++
CSRC    = $(SRC)
TARGET  = blackscholes
LDFLAGS = -L$(TBBROOT)/obj
#LIBS    = -ltbb -ltbbmalloc_proxy -ltbbmalloc -ldl -lrt -lpthread
LIBS = -ltbb
CXXFLAGS += -I$(TBBROOT)/include

SRC	= blackscholes.c
MT        = -DENABLE_TBB

# Default build single precision version
NCO     = -DNCO=4

CXXFLAGS += $(MT) $(NCO) $(FUNC) $(ERR) $(CSRC)

all: $(TARGET)

$(TARGET): clean $(CSRC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET) *~ *.out prices.txt
