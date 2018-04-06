#include "t_debug_task.h"

class quicksort_task: public tbb::t_debug_task {
    /*override*/tbb::task* execute();
    T *first, *last;
public:
    quicksort_task( T* first_, T* last_ ) : first(first_), last(last_) {}
};

tbb::task* quicksort_task::execute() {
  __exec_begin__(getTaskId(),__FILE__,__LINE__);

  if( last-first<=QUICKSORT_CUTOFF ) {
    std::sort(first,last);
    __exec_end__(getTaskId(),__FILE__,__LINE__);
    return NULL;
  } else {
    // Divide
    T* middle = divide(first,last);
    if( !middle ) {
      __exec_end__(getTaskId(),__FILE__,__LINE__);
      return NULL; 
    }
    
    // Now have two subproblems: [first..middle) and [middle+1..last)
    set_ref_count(3);
    
    tbb::t_debug_task* smaller = new( task::allocate_child() ) quicksort_task( first, middle );
    tbb::t_debug_task* larger = new( task::allocate_child() ) quicksort_task( middle+1, last );
    
    tbb::t_debug_task::spawn(*smaller, __FILE__, __LINE__);
    tbb::t_debug_task::spawn(*larger, __FILE__, __LINE__);
    
    tbb::t_debug_task::wait_for_all(__FILE__, __LINE__);
    
    __exec_end__(getTaskId(),__FILE__, __LINE__);
    return NULL;
  }
}

void parallel_quicksort( T* first, T* last ) {
  // Create root task
  tbb::t_debug_task& t = *new( tbb::task::allocate_root() ) quicksort_task( first, last );
  // Run it
  tbb::t_debug_task::spawn_root_and_wait(t, __FILE__, __LINE__);
}
