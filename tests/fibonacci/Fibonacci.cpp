/*
    Copyright 2005-2014 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks. Threading Building Blocks is free software;
    you can redistribute it and/or modify it under the terms of the GNU General Public License
    version 2  as  published  by  the  Free Software Foundation.  Threading Building Blocks is
    distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See  the GNU General Public License for more details.   You should have received a copy of
    the  GNU General Public License along with Threading Building Blocks; if not, write to the
    Free Software Foundation, Inc.,  51 Franklin St,  Fifth Floor,  Boston,  MA 02110-1301 USA

    As a special exception,  you may use this file  as part of a free software library without
    restriction.  Specifically,  if other files instantiate templates  or use macros or inline
    functions from this file, or you compile this file and link it with other files to produce
    an executable,  this file does not by itself cause the resulting executable to be covered
    by the GNU General Public License. This exception does not however invalidate any other
    reasons why the executable file might be covered by the GNU General Public License.
*/

/* Example program that computes Fibonacci numbers in different ways.
   Arguments are: [ Number [Threads [Repeats]]]
   The defaults are Number=500 Threads=1:4 Repeats=1.

   The point of this program is to check that the library is working properly.
   Most of the computations are deliberately silly and not expected to
   show any speedup on multiprocessors.
*/

// enable assertions
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <utility>
#include "tbb/task.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"
#include "tbb/blocked_range.h"
#include "tbb/tbb_thread.h"

#include "t_debug_task.h"

using namespace std;
using namespace tbb;

//! type used for Fibonacci number computations
typedef long long value;

// *** Raw tasks *** //

//! task class which computes Fibonacci numbers by Lucas formula
struct FibTask: public t_debug_task {
  const int n;
  value& sum;
  value x, y;
  // task arguments
  FibTask( int n_, value& sum_ ) : 
    n(n_), sum(sum_)
  {}

  //! Execute task
  /*override*/ task* execute() {
    __exec_begin__(getTaskId());
    if( n < 2 ) {
      sum = n;
      __exec_end__(getTaskId());
      return NULL;
    } else {
      set_ref_count(3);
      FibTask& a = *new( allocate_child() ) FibTask( n-1, x );
      FibTask& b = *new( allocate_child() ) FibTask( n-2, y );
      t_debug_task::spawn( a, __FILE__, __LINE__ );
      t_debug_task::spawn( b, __FILE__, __LINE__ );
      t_debug_task::wait_for_all();
      sum = x + y;
      __exec_end__(getTaskId());
      return NULL;
    }
  }
};

//! Root function
value ParallelTaskFib(int n) { 
    value sum;
    FibTask& a = *new(task::allocate_root()) FibTask(n, sum);
    t_debug_task::spawn_root_and_wait(a, __FILE__, __LINE__);
    return sum;
}

/////////////////////////// Main ////////////////////////////////////////////////////

//! Tick count for start
static tick_count t0;

//! Verbose output flag
static bool Verbose = false;

typedef value (*MeasureFunc)(int);
//! Measure ticks count in loop [2..n]
value Measure(const char *name, MeasureFunc func, int n)
{
    value result;
    if(Verbose) printf("%s",name);
    t0 = tick_count::now();
    for(int number = 2; number <= n; number++)
        result = func(number);
    if(Verbose) printf("\t- in %f msec\n", (tick_count::now() - t0).seconds()*1000);
    return result;
}

//! program entry
int main(int argc, char* argv[])
{
  TD_Activate();
  if(argc>1) Verbose = true;
  int NumbersCount = argc>1 ? strtol(argv[1],0,0) : 25;

  task_scheduler_init scheduler_init(16);
  value sum = Measure("Parallel tasks\t\t", ParallelTaskFib, NumbersCount);
    
  Fini();
  return 0;
}
