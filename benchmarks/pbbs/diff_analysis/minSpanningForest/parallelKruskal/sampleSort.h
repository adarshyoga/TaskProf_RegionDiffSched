// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2010 Guy Blelloch and Harsha Vardhan Simhadri and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <iostream>
#include <algorithm>
#include "parallel.h"
#include "utils.h"
#include "sequence.h"
#include "gettime.h"
#include "math.h"
#include "quickSort.h"
#include "transpose.h"

template<class E, class BinPred, class intT>
void mergeSeq (E* sA, E* sB, intT* sC, intT lA, intT lB, BinPred f) {
  if (lA==0 || lB==0) return;
  E *eA = sA+lA;
  E *eB = sB+lB;
  for (intT i=0; i <= lB; i++) sC[i] = 0;
  while(1) {
    while (f(*sA, *sB)) {(*sC)++; if (++sA == eA) return;}
    sB++; sC++;
    if (sB == eB) break;
    if (!(f(*(sB-1),*sB))) {
      while (!f(*sB, *sA)) {(*sC)++; if (++sA == eA) return;}
      sB++; sC++;
      if (sB == eB) break;
    }
  } 
  *sC = eA-sA;
}

#define SSORT_THR 100000
#define AVG_SEG_SIZE 2
#define PIVOT_QUOT 2

template<class E, class BinPred, class intT>
void sampleSort (E* A, intT n, BinPred f) {
  if (n < SSORT_THR) compSort(A, n, f);  
  else {
    intT sq = (intT)(pow(n,0.5));
    intT rowSize = sq*AVG_SEG_SIZE;
    intT numR = (intT)ceil(((double)n)/((double)rowSize));
    intT numSegs = (sq-1)/PIVOT_QUOT;
    int overSample = 4;
    intT sampleSetSize = numSegs*overSample;
    E* sampleSet = newA(E,sampleSetSize);
    //cout << "n=" << n << " num_segs=" << numSegs << endl;

    // generate samples with oversampling
    /* parallel_for (intT j=0; j< sampleSetSize; ++j) { */
    /*   intT o = utils::hash(j)%n; */
    /*   sampleSet[j] = A[o];  */
    /* } */
    parallel_for(
		 tbb::blocked_range<intT>(0, sampleSetSize,6),
		 [=](tbb::blocked_range<intT> r, size_t thdId)
		 {for( intT j=r.begin(); j!=r.end(); ++j ) {
		     intT o = utils::hash(j)%n;
		     sampleSet[j] = A[o];
		   }
		 }, tbb::simple_partitioner(), __FILE__, __LINE__
		 );

    // sort the samples
    compSort(sampleSet, sampleSetSize, f);

    // subselect samples at even stride
    E* pivots = newA(E,numSegs-1);
    /* parallel_for (intT k=0; k < numSegs-1; ++k) { */
    /*   intT o = overSample*k; */
    /*   pivots[k] = sampleSet[o]; */
    /* } */
    parallel_for(
		 tbb::blocked_range<intT>(0, numSegs-1,1),
		 [=](tbb::blocked_range<intT> r, size_t thdId)
		 {for( intT k=r.begin(); k!=r.end(); ++k ) {
		     intT o = overSample*k;
		     pivots[k] = sampleSet[o];
		   }
		 }, tbb::simple_partitioner(), __FILE__, __LINE__
		 );

    free(sampleSet);  
    //nextTime("samples");

    E *B = newA(E, numR*rowSize);
    intT *segSizes = newA(intT, numR*numSegs);
    intT *offsetA = newA(intT, numR*numSegs);
    intT *offsetB = newA(intT, numR*numSegs);

    // sort each row and merge with samples to get counts
    /* parallel_for (intT r = 0; r < numR; ++r) { */
    /*   intT offset = r * rowSize; */
    /*   intT size =  (r < numR - 1) ? rowSize : n - offset; */
    /*   sampleSort(A+offset, size, f); */
    /*   mergeSeq(A + offset, pivots, segSizes + r*numSegs, size, numSegs-1, f); */
    /* } */
    parallel_for(
		 tbb::blocked_range<intT>(0, numR,1),
		 [=](tbb::blocked_range<intT> rg, size_t thdId)
		 {for( intT r=rg.begin(); r!=rg.end(); ++r ) {
		     intT offset = r * rowSize;
		     intT size =  (r < numR - 1) ? rowSize : n - offset;
		     sampleSort(A+offset, size, f);
		     mergeSeq(A + offset, pivots, segSizes + r*numSegs, size, numSegs-1, f);
		   }
		 }, tbb::simple_partitioner(), __FILE__, __LINE__
		 );
    //nextTime("sort and merge");

    // transpose from rows to columns
    sequence::scan(segSizes, offsetA, numR*numSegs, plus<intT>(),(intT)0);
    transpose<intT,intT>(segSizes, offsetB).trans(numR, numSegs);
    sequence::scan(offsetB, offsetB, numR*numSegs, plus<intT>(),(intT)0);
    blockTrans<E,intT>(A, B, offsetA, offsetB, segSizes).trans(numR, numSegs);
    //{parallel_for (intT i=0; i < n; ++i) A[i] = B[i];}
    parallel_for(
		 tbb::blocked_range<intT>(0, n,13552),
		 [=](tbb::blocked_range<intT> r, size_t thdId)
		 {for( intT i=r.begin(); i!=r.end(); ++i ) {
		     A[i] = B[i];
		   }
		 }, tbb::simple_partitioner(), __FILE__, __LINE__
		 );

    //nextTime("transpose");

    free(B); free(offsetA); free(segSizes);

    // sort the columns
    /* {parallel_for (intT i=0; i<numSegs; ++i) { */
    /* 	intT offset = offsetB[i*numR]; */
    /* 	if (i == 0) { */
    /* 	  sampleSort(A, offsetB[numR], f); // first segment */
    /* 	} else if (i < numSegs-1) { // middle segments */
    /* 	  // if not all equal in the segment */
    /* 	  if (f(pivots[i-1],pivots[i]))  */
    /* 	    sampleSort(A+offset, offsetB[(i+1)*numR] - offset, f); */
    /* 	} else { // last segment */
    /* 	  sampleSort(A+offset, n - offset, f); */
    /* 	} */
    /*   } */
    /* } */
    parallel_for(
		 tbb::blocked_range<intT>(0, numSegs,1),
		 [=](tbb::blocked_range<intT> r, size_t thdId)
		 {for( intT i=r.begin(); i!=r.end(); ++i ) {
		     intT offset = offsetB[i*numR];
		     if (i == 0) {
		       sampleSort(A, offsetB[numR], f); // first segment
		     } else if (i < numSegs-1) { // middle segments
		       // if not all equal in the segment
		       if (f(pivots[i-1],pivots[i])) 
			 sampleSort(A+offset, offsetB[(i+1)*numR] - offset, f);
		     } else { // last segment
		       sampleSort(A+offset, n - offset, f);
		     }
		   }
		 }, tbb::simple_partitioner(), __FILE__, __LINE__
		 );

    //nextTime("last sort");
    free(pivots); free(offsetB);
  }
}

#undef compSort
#define compSort(__A, __n, __f) (sampleSort(__A, __n, __f))

