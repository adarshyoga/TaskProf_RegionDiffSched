#ifndef TASK_PROFILER_H
#define TASK_PROFILER_H

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <sys/syscall.h>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

#include "AFTaskGraph.H"
#include "Common.H"
#include "Location.H"
#include "Task_Logger.H"

extern AFTaskGraph* taskGraph;
extern Task_Logger* taskLogger;

enum CounterType {
  CYCLES = 0,
  INSTRUCTIONS = 1,
  LOCAL_HITM = 2,
  REMOTE_HITM = 3,
  LOCAL_DRAM = 4,
  REMOTE_DRAM = 5,
  PAGE_FAULTS = 6,
  LLC_MISS = 7,
  L1_M_REPLACE = 8,
  L1_EVICTION = 9,
  L1_REPLACEMENT = 10,
  FP_DIVIDE = 11
};

struct sched_overhead_data {
  size_t sched_ohead_work;
  size_t sched_calls;
};

class Task_Profiler {
private:
  int p_counter_type;
  std::unordered_map<size_t, struct CallSiteData*> regionMap;
  Task_Logger t_logger;

  struct sched_overhead_data so_data[NUM_THREADS] = {{0,0}};

  int perf_fds[NUM_THREADS];
  int perf_event_open_wrapper(struct perf_event_attr *hw_event, pid_t pid,
			      int cpu, int group_fd, unsigned long flags);
  size_t stop_n_get_count (THREADID threadid);
  void start_count(THREADID threadid);

public:
  Task_Profiler(int counter_type);
  void set_counter_type(struct perf_event_attr* pe);
  void TP_CaptureExecute(THREADID threadid);
  void TP_CaptureReturn(THREADID threadid);
  void TP_CaptureWait_Entry(THREADID threadid);
  void TP_CaptureWait_Exit(THREADID threadid);
  void TP_CaptureSpawn_Entry(THREADID threadid);
  void TP_CaptureSpawn_Exit(THREADID threadid);
  void TP_CaptureBeginOptimize(THREADID threadid, const char* file, int line, void* return_address);
  void TP_CaptureEndOptimize(THREADID threadid, const char* file, int line, void* return_address);
  void Fini();
  void dumpRegionInfo();
  void update_step_and_region(THREADID threadid);

  void startSchedCount(THREADID threadid);
  void stopSchedCount(THREADID threadid);
  size_t stopSchedCountOnly(THREADID threadid);
};

#endif
