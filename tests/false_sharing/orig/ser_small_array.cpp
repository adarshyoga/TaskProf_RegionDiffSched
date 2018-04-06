#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include <math.h>
#include <iostream>

using namespace tbb;

#define ARRAY_SIZE 64

char a[ARRAY_SIZE] __attribute__ ((aligned));

void SerialApplyFoo( char a[], size_t n ) {
  for( size_t i=0; i!=n; ++i ) { 
    for (int j=0;j < 100000000;j++)
      a[i]++;
    for (int j=0;j < 100000000;j++)
      a[i]--;
  }
}

int main(int argc, char** argv) {
  //tbb::task_scheduler_init init(16);

  SerialApplyFoo(a, ARRAY_SIZE);
}
