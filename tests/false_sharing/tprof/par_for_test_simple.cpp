#include <stdlib.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"

using namespace tbb;

#define ARRAY_SIZE 1000000
#define GRAINSIZE 1

float a[ARRAY_SIZE];

class ApplyFoo {
public:
  void operator()( const blocked_range<size_t>& r, size_t taskId ) const {
    for( size_t i=r.begin(); i!=r.end(); ++i ) { 
      for (int j=0;j < 500;j++)
	a[i]++;
    }
  }
};

void ParallelApplyFoo( float a[], size_t n ) {
  parallel_for(blocked_range<size_t>(0,n,GRAINSIZE), ApplyFoo(), tbb::simple_partitioner(), __FILE__, __LINE__);
}

int main(int argc, char** argv) {
  tbb::task_scheduler_init init(16);
  TD_Activate(__FILE__,__LINE__);
  ParallelApplyFoo(a, ARRAY_SIZE);
  Fini(__FILE__,__LINE__);
}
