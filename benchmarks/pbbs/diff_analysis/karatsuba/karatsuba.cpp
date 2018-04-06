#include <cstdlib>
#include <cassert>
#include <cstdio>
#include "t_debug_task.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"

// Assumed to hold 32 bits
typedef unsigned int T;

#include "temp_space.h"

#include <tbb/parallel_invoke.h>

namespace TBBExample {
#include "simple_mul_serial.h"
#include "karatsuba.h"
}

const double MinTime = 2;
const size_t nMax = 10000;

static void FillRandom( T x[], size_t n ) {
    const unsigned Radix = 10;
    for( size_t k=0; k<n; ++k ) 
        x[k] = std::rand() % Radix;
}

static void TestKaratsuba() {
    for( size_t trial=0; trial<4; ++trial ) {
        for( size_t n=1; n<nMax; n*=3 ) {
            T x[nMax];
            T y[nMax];
            T z[3][2*nMax];
            FillRandom(x,n);
            FillRandom(y,n);
            int m = 0;  
            //SerialExample::karatsuba(z[m++],x,y,n);
            TBBExample::karatsuba(z[m++],x,y,n);
            T w[2*nMax-1];
            TBBExample::simple_mul(w,x,y,n);
            // Check the results
            for( size_t k=0; k<m; ++k )
	      for( size_t i=0; i<2*n-1; ++i )
		assert( w[i]==z[k][i] );
        }
    }
}

typedef void (*Routine)( T c[], const T a[], const T b[], size_t n );

static void TimeRoutine( Routine r, const char* what ) {
    // First call is used to choose how many iterations to time.
    static int TrialCount;
    bool firstCall = TrialCount==0;

    T x[nMax];
    T y[nMax];
    T z[2*nMax];
    FillRandom(x,nMax);
    FillRandom(y,nMax);
    if( firstCall ) 
        TrialCount=1;
    double t;
    for(;;) {
        tbb::tick_count t0 = tbb::tick_count::now();
        for( int i=0; i<TrialCount; ++i ) 
            (*r)(z,x,y,nMax);
        tbb::tick_count t1 = tbb::tick_count::now();
        t = (t1-t0).seconds();
        if( !firstCall || t>=MinTime ) break;
        // Double the number of iterations
        TrialCount*=2;
    } 
    if( firstCall ) {
        // Print table caption and heading
        printf("Timing %d multiplications of %lu-degree polynomials\n\n",TrialCount,(unsigned long)nMax);
        printf("%32s %s\n","Version","Time");
    }
    std::printf("%32s %g\n",what,t);
}

// Test for karatsuba routines
int main ( int argc, char* argv[] ) {
  tbb::task_scheduler_init init(atoi(argv[1]));
  TD_Activate(__FILE__,__LINE__);
  std::srand(2);
  // Test for correctness
  printf("Testing Karatsuba implementations...\n");
  TestKaratsuba();

  // Test speed.  The correctness tests are assumed to have warmed up the runtimes.
  //TimeRoutine( SerialExample::simple_mul, "flat algorithm (serial)" );
  //TimeRoutine( SerialExample::karatsuba, "Karatsuba algorithm (serial)" );
  TimeRoutine( TBBExample::karatsuba, "Karatsuba algorithm in TBB" );
  Fini(__FILE__,__LINE__);
}
