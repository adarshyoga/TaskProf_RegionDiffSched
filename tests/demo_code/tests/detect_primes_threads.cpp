#include <fstream>
#include <iostream>
#include <assert.h>

#define NUM_THREADS 4

size_t* primes[NUM_THREADS];
size_t num_primes[NUM_THREADS];

struct args {
  int tid;
  int lb;
  int ub;
};

bool IsPrime(size_t num) {
  if (num <= 1) return false; // zero and one are not prime
  for (size_t i=2; i*i<=num; i++) {
    if (num % i == 0) return false;
  }
  return true;
}

void* check_primes(void *args) {
  struct args* arguments = (struct args*)args;
  
  for (size_t i = arguments->lb; i < arguments->ub; i++) {
    if (IsPrime(i)) {
      primes[arguments->tid][num_primes[arguments->tid]] = i;
      num_primes[arguments->tid]++;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Format: ./detect_primes <range_begin> <range_end>" << std::endl;
    return 0;
  }

  size_t range_begin = strtoul(argv[1], NULL, 0);
  size_t range_end = strtoul(argv[2], NULL, 0);
  assert(range_end > range_begin);

  pthread_t threadpool[NUM_THREADS];
  struct args arguments[NUM_THREADS];

  for (unsigned int i = 0; i < NUM_THREADS; i++) {
    primes[i] = new size_t[range_end-range_begin];
    arguments[i].tid = i;
    arguments[i].lb = ((range_end-range_begin)/NUM_THREADS)*i;
    arguments[i].ub = ((range_end-range_begin)/NUM_THREADS)*(i+1);

    pthread_create(&threadpool[i], NULL, check_primes, &arguments[i]);
  }

  for (unsigned int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threadpool[i], NULL);
  }

  std::ofstream report;
  report.open("primes.txt");
  for (unsigned int i = 0; i < NUM_THREADS; i++) {
    for (size_t j = 0; j < num_primes[i]; j++)
      report << primes[i][j] << "   ";
  }
}
