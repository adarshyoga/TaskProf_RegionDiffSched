#include "geometry.h"
#include "sequence.h"

class Hull:public tbb::task {
 private:
  _seq<intT>* ret_val;
  point2d* P;
  intT n;
 public:
  tbb::task* execute();
 Hull(_seq<intT>* ret_val, point2d* P, intT n):
  ret_val(ret_val), P(P), n(n) {}
};

class QuickHull:public tbb::task {
private:
  intT* ret_val;
  intT* I;
  intT* Itmp;
  point2d* P;
  intT n;
  intT l;
  intT r;
  intT depth;
public:
  tbb::task* execute();
 QuickHull(intT* ret_val,
	   intT* I,
	   intT* Itmp,
	   point2d* P,
	   intT n,
	   intT l,
	   intT r,
	   intT depth):
  ret_val(ret_val), I(I), Itmp(Itmp), P(P), n(n), l(l), r(r), depth(depth)
  {}
};

//_seq<intT> hull(point2d*, intT);

