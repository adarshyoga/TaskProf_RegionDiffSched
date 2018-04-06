#include "kmeans.h"

void repair_empty_clusters( size_t n, const point points[], cluster_id id[], size_t k, point centroid[], sum_and_count sum[] ) {
  for( size_t j=0; j<k; ++j ) {
    if( sum[j].count==0 ) {
      // Find index of point that furthest away from its cluster.
      // Array notation is not available, so write out the loops.
      size_t farthest = -1;
      float maxd = -1;
      for( int i=0; i<n; ++i ) {
	float d = distance2(centroid[id[i]],points[i]);
	if( d>maxd  ) {
	  maxd = d;
	  farthest = i;
	}
      }
      id[farthest] = j;
      sum[j].count = 1;
      sum[j].sum = points[farthest];
    } 
  }
}
