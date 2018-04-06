#include<iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/task.h"

using namespace std;
using namespace tbb;

#define PAR_RANGE 100000000
#define GRAINSIZE 1

struct data {
  char d;
  char pad[63];
};

struct data a[16] __attribute__ ((aligned));

class Driver: public task {
  size_t size;
public:
  Driver(size_t s) {
    size = s;
  }
  
  task* execute() {
    if (size <= GRAINSIZE) {
      a[10].d++;
    } else {
 
      set_ref_count(3);

      task& a = *new(task::allocate_child()) Driver(size/2);
      spawn(a);

      task& b = *new(task::allocate_child()) Driver(size/2);
      spawn(b);
    
      wait_for_all();
    }
    
    return NULL;
  }
};
 
int main( int argc, const char *argv[] ) {
  tbb::task_scheduler_init init(atoi(argv[1]));

  task& v = *new(task::allocate_root()) Driver(PAR_RANGE);
  task::spawn_root_and_wait(v);
}
