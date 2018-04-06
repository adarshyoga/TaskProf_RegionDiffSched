#include <iostream>
#include <string.h>
//#include <assert.h>
#include "AFTaskGraph.H"

AFTaskGraph::AFTaskGraph() {
  //report_int_nodes.open("int_nodes.csv");
  last_allocated_node = 1;

  struct AFTask* head_node = create_node(0);
  //head_node->ut_id.seq_num_str = "1";
  cur_dpst_index[0].push(head_node);

  taskLogger->log(0,head_node->node_id,0,0,FINISH,0);
  // report_int_nodes << 0 << ","
  // 		   << FINISH << ","
  // 		   << 0
  // 		   << std::endl;

  //Add step node as the first child of Head node
  head_node->child_id = last_allocated_node;
  last_allocated_node++;
}

struct AFTask* AFTaskGraph::create_node(THREADID threadid) {
  //initialize task;
  //new_node->ut_id.depth = 0;
  // new_node->ut_id.sum_seq_nos = 0;

  struct AFTask* new_node = new AFTask();
  new_node->node_id = last_allocated_node;
  last_allocated_node++;
  //new_node->num_children = 0;
  new_node->young_ns_child = NO_TYPE;
  new_node->sync_finish_flag = false;

  return new_node;
}

size_t AFTaskGraph::getCurStepParent(THREADID threadid) {
  //struct unique_task_id step_ut_id;
  //step_ut_id.seq_num_str = cur_dpst_index[threadid].top()->ut_id.seq_num_str + "." + 
  //std::to_string(cur_dpst_index[threadid].top()->num_children);
  // step_ut_id.sum_seq_nos = cur_dpst_index[threadid].top()->ut_id.sum_seq_nos + 
  //   cur_dpst_index[threadid].top()->num_children;
  //step_ut_id.depth = cur_dpst_index[threadid].top()->ut_id.depth + 1;
  //return step_ut_id;
  return (cur_dpst_index[threadid].top())->node_id;
}

size_t AFTaskGraph::getCurStepId(THREADID threadid) {
  return (cur_dpst_index[threadid].top())->child_id;
}

std::string AFTaskGraph::getCurSeqNoStr(THREADID threadid) {
  // std::string seq_num_str = cur_dpst_index[threadid].top()->ut_id.seq_num_str + "." + 
  //   std::to_string(cur_dpst_index[threadid].top()->num_children);
  // return seq_num_str;
  return "0";
}

struct location AFTaskGraph::getCurStepRegionBegin(THREADID threadid) {
  struct AFTask* cur_task  = cur_dpst_index[threadid].top();
  return cur_task->start;
}

struct location AFTaskGraph::getCurStepRegionEnd(THREADID threadid) {
  struct AFTask* cur_task  = cur_dpst_index[threadid].top();
  return cur_task->end;
}

void AFTaskGraph::attributeSchedOvWork(THREADID threadid, size_t so_work) {
  struct AFTask* cur_task  = cur_dpst_index[threadid].top();
  cur_task->sc_ov_work += so_work;
}

void AFTaskGraph::CaptureSpawnOnly(THREADID threadid, size_t taskId, void* return_address)
{
  PIN_GetLock(&lock, 0);

  struct AFTask* cur_node = cur_dpst_index[threadid].top();

  //if current node rightmost child is Async or not
  //check for STEP and prev of STEP == ASYNC
  struct AFTask* newNode;
  if (cur_node->young_ns_child == ASYNC) {
    newNode = create_node(threadid);
    cur_node->young_ns_child = ASYNC;

    //cur_node->num_children++;
    //newNode->ut_id.seq_num_str = cur_node->ut_id.seq_num_str + "." + std::to_string(cur_node->num_children);
    //newNode->ut_id.depth = cur_node->ut_id.depth + 1;

    taskLogger->log(threadid,newNode->node_id,cur_node->node_id,0,ASYNC,(ADDRINT)return_address);
    // report_int_nodes << cur_node->node_id << ","
    // 		     << ASYNC << ","
    // 		     << (ADDRINT)return_address
    // 		     << std::endl;
  } else {
    struct AFTask* newFinish = create_node(threadid);
    //cur_node->num_children++;
    cur_node->young_ns_child = FINISH;

    newFinish->sync_finish_flag = true;
    //newFinish->ut_id.seq_num_str = cur_node->ut_id.seq_num_str + "." + std::to_string(cur_node->num_children);
    //newFinish->ut_id.depth = cur_node->ut_id.depth + 1;

    taskLogger->log(threadid,newFinish->node_id,cur_node->node_id,0,FINISH,0);
    // report_int_nodes << cur_node->node_id << ","
    // 		     << FINISH << ","
    // 		     << 0
    // 		     << std::endl;

    newNode = create_node(threadid);
    //newFinish->num_children++;
    newFinish->young_ns_child = ASYNC;

    //newNode->ut_id.seq_num_str = newFinish->ut_id.seq_num_str + "." + std::to_string(newFinish->num_children);
    //newNode->ut_id.depth = newFinish->ut_id.depth + 1;

    taskLogger->log(threadid,newNode->node_id,newFinish->node_id,0,ASYNC,(ADDRINT)return_address);
    // report_int_nodes << newFinish->node_id << ","
    // 		     << ASYNC << ","
    // 		     << (ADDRINT)return_address
    // 		     << std::endl;

    cur_dpst_index[threadid].push(newFinish);
    cur_node = newFinish;
  }

  newNode->child_id = last_allocated_node;
  last_allocated_node++;
  // add newNode->children STEP and cur_node->children STEP
  // size_t newStep_index = create_node(STEP, newNode->taskId);
  // struct AFTask* newStep = &tgraph_nodes[newStep_index];
  // newStep->parent = newNode_index;

  //newNode->num_children++;

  cur_node->child_id = last_allocated_node;
  last_allocated_node++;  
  // newStep_index = create_node(STEP, cur_node->taskId);
  // newStep = &tgraph_nodes[newStep_index];
  // newStep->parent = cur_dpst_index[threadid].top();

  //cur_node->num_children++;
  //assert(temp_cur_map.count(taskId) != 0);
  temp_cur_map.insert(std::pair<size_t, struct AFTask*>(taskId, newNode));

  PIN_ReleaseLock(&lock);
}

void AFTaskGraph::CaptureSpawnAndWaitForAll(THREADID threadid, size_t taskId, void* return_address)
{
  PIN_GetLock(&lock, 0);

  struct AFTask* cur_node = cur_dpst_index[threadid].top();

  //if current node rightmost child is Async or not
  //check for STEP and prev of STEP == ASYNC
  struct AFTask* newNode;
  if (cur_node->young_ns_child == ASYNC) {
    newNode = create_node(threadid);
    cur_node->young_ns_child = ASYNC;

    //cur_node->num_children++;
    //newNode->ut_id.seq_num_str = cur_node->ut_id.seq_num_str + "." + std::to_string(cur_node->num_children);
    //newNode->ut_id.depth = cur_node->ut_id.depth + 1;

    taskLogger->log(threadid,newNode->node_id,cur_node->node_id,0,ASYNC,(ADDRINT)return_address);
    // report_int_nodes << cur_node->node_id << ","
    // 		     << ASYNC << ","
    // 		     << (ADDRINT)return_address
    // 		     << std::endl;
  } else {
    struct AFTask* newFinish = create_node(threadid);
    //cur_node->num_children++;
    cur_node->young_ns_child = FINISH;

    newFinish->sync_finish_flag = true;
    //newFinish->ut_id.seq_num_str = cur_node->ut_id.seq_num_str + "." + std::to_string(cur_node->num_children);
    //newFinish->ut_id.depth = cur_node->ut_id.depth + 1;

    taskLogger->log(threadid,newFinish->node_id,cur_node->node_id,0,FINISH,0);
    // report_int_nodes << cur_node->node_id << ","
    // 		     << FINISH << ","
    // 		     << 0
    // 		     << std::endl;

    newNode = create_node(threadid);
    //newFinish->num_children++;
    newFinish->young_ns_child = ASYNC;

    //newNode->ut_id.seq_num_str = newFinish->ut_id.seq_num_str + "." + std::to_string(newFinish->num_children);
    //newNode->ut_id.depth = newFinish->ut_id.depth + 1;

    taskLogger->log(threadid,newNode->node_id,newFinish->node_id,0,ASYNC,(ADDRINT)return_address);
    // report_int_nodes << newFinish->node_id << ","
    // 		     << ASYNC << ","
    // 		     << (ADDRINT)return_address
    // 		     << std::endl;

    cur_dpst_index[threadid].push(newFinish);
    cur_node = newFinish;
  }

  newNode->child_id = last_allocated_node;
  last_allocated_node++;
  // add newNode->children STEP and cur_node->children STEP
  // size_t newStep_index = create_node(STEP, newNode->taskId);
  // struct AFTask* newStep = &tgraph_nodes[newStep_index];
  // newStep->parent = newNode_index;

  //newNode->num_children++;

  //assert(temp_cur_map.count(taskId) != 0);
  temp_cur_map.insert(std::pair<size_t, struct AFTask*>(taskId, newNode));

  PIN_ReleaseLock(&lock);
}

void AFTaskGraph::CaptureExecute(THREADID threadid, size_t taskId)
{
  PIN_GetLock(&lock, 0);
  struct AFTask* temp_cur = temp_cur_map.at(taskId);
  temp_cur_map.erase(taskId);
  //assert (temp_cur != NULL);
  cur_dpst_index[threadid].push(temp_cur);
  PIN_ReleaseLock(&lock);
}

void AFTaskGraph::CaptureSpawnAndWait(THREADID threadid, size_t taskId, void* return_address)
{
  PIN_GetLock(&lock, 0);

  struct AFTask* cur_node = cur_dpst_index[threadid].top();
  struct AFTask* newFinish = create_node(threadid);
  //newFinish->parent = cur_index;
  //cur_node->num_children++;
  cur_node->young_ns_child = FINISH;

  //newFinish->ut_id.seq_num_str = cur_node->ut_id.seq_num_str + "." + std::to_string(cur_node->num_children);
  //newFinish->ut_id.depth = cur_node->ut_id.depth + 1;

  taskLogger->log(threadid,newFinish->node_id,cur_node->node_id,0,FINISH,(ADDRINT)return_address);  
  // report_int_nodes << cur_node->node_id << ","
  // 		   << FINISH << ","
  // 		   << (size_t) return_address
  // 		   << std::endl;

  // size_t newStep_index = create_node(STEP, newFinish->taskId);
  // struct AFTask* newStep = &tgraph_nodes[newStep_index];
  // newStep->parent = newFinish_index;
  newFinish->child_id = last_allocated_node;
  last_allocated_node++;

  //newFinish->num_children++;
  //assert(temp_cur_map.count(taskId) != 0);
  temp_cur_map.insert(std::pair<size_t, struct AFTask*>(taskId,newFinish));
  
  PIN_ReleaseLock(&lock);
}

void AFTaskGraph::CaptureReturn(THREADID threadid)
{
  //PIN_GetLock(&lock, 0);
  struct AFTask* cur_node = cur_dpst_index[threadid].top();
  if (cur_node->sync_finish_flag == true) {
    taskLogger->log_sched_ov(threadid, cur_node->node_id, cur_node->sc_ov_work);
    cur_dpst_index[threadid].pop();      
    delete cur_node->start.filename;
    cur_node->start.filename = NULL;
    delete cur_node->end.filename;
    cur_node->end.filename = NULL;
    delete cur_node;
    cur_node = cur_dpst_index[threadid].top();
  }
  taskLogger->log_sched_ov(threadid,
			   (cur_dpst_index[threadid].top())->node_id,
			   (cur_dpst_index[threadid].top())->sc_ov_work);
  cur_dpst_index[threadid].pop();
  delete cur_node->start.filename;
  delete cur_node->end.filename;  
  delete cur_node;
  //PIN_ReleaseLock(&lock);
}

void AFTaskGraph::CaptureWait(THREADID threadid)
{
  PIN_GetLock(&lock, 0);
  
  struct AFTask* cur_node = cur_dpst_index[threadid].top();

  cur_node->child_id = last_allocated_node;
  last_allocated_node++;
  // size_t newStep_index = create_node(STEP, cur_node->taskId);
  // struct AFTask* newStep = &tgraph_nodes[newStep_index];
  // newStep->parent = cur_index;

  //cur_node->num_children++;
  //cur_node->cur_step = newStep_index;

  PIN_ReleaseLock(&lock);
}

void AFTaskGraph::CaptureWaitOnly(THREADID threadid)
{
  PIN_GetLock(&lock, 0);
  struct AFTask* cur_node = cur_dpst_index[threadid].top();

  taskLogger->log_sched_ov(threadid, cur_node->node_id, cur_node->sc_ov_work);
  
  cur_dpst_index[threadid].pop();
  delete cur_node->start.filename;
  cur_node->start.filename = NULL;
  delete cur_node->end.filename;
  cur_node->end.filename = NULL;
  delete cur_node;

  cur_node = cur_dpst_index[threadid].top();

  cur_node->child_id = last_allocated_node;
  last_allocated_node++;  
  // size_t newStep_index = create_node(STEP, cur_node->taskId);
  // struct AFTask* newStep = &tgraph_nodes[newStep_index];
  // newStep->parent = cur_index;

  //cur_node->num_children++;
  //cur_node->cur_step = newStep_index;
  
  PIN_ReleaseLock(&lock);
}

void AFTaskGraph::CaptureSetTaskId(THREADID threadid, size_t taskId,
				   int sp_only, void* return_address, 
				   const char* file, int line, bool par_for) {
  if (line == 0 || file == NULL) {
    return_address = 0;
  } else {
    if (callSiteMap.count((ADDRINT)return_address) == 0) {
      struct CallSiteData* callsiteData = new CallSiteData();
      callsiteData->cs_filename = file;
      callsiteData->cs_line_number = line;
      callsiteData->par_for = par_for;
      callSiteMap.insert(std::pair<size_t, struct CallSiteData*>((ADDRINT)return_address, callsiteData));
    }
  }

  if (sp_only) {
    CaptureSpawnOnly(threadid, taskId, return_address);
  } else {
    CaptureSpawnAndWait(threadid, taskId, return_address);
  }
}

void AFTaskGraph::CaptureSetTaskId(THREADID threadid, size_t taskId,
				   void* return_address, 
				   const char* file, int line, bool par_for) {
  if (line == 0 || file == NULL) {
    return_address = 0;
  } else {
    if (callSiteMap.count((ADDRINT)return_address) == 0) {
      struct CallSiteData* callsiteData = new CallSiteData();
      callsiteData->cs_filename = file;
      callsiteData->cs_line_number = line;
      callsiteData->par_for = par_for;
      callSiteMap.insert(std::pair<size_t, struct CallSiteData*>((ADDRINT)return_address, callsiteData));
    }
  }

  CaptureSpawnAndWaitForAll(threadid, taskId, return_address);
}

void AFTaskGraph::dumpCallsiteInfo() {
  std::ofstream report_callsite_info;
  report_callsite_info.open("callsite_info.csv");
  for (std::unordered_map<size_t,struct CallSiteData*>::iterator it=callSiteMap.begin();
	 it!=callSiteMap.end(); ++it) {
    struct CallSiteData* cs_data = it->second;
    report_callsite_info << it->first << ","
			 << cs_data->cs_filename << ","
			 << cs_data->cs_line_number << ","
			 << cs_data->par_for
			 << std::endl;
  }
  report_callsite_info.close();
}

// struct CallSiteData* AFTaskGraph::getSourceFileAndLine(size_t return_address) {
//   return callSiteMap.at(return_address);
// }

void AFTaskGraph::setStepRegion(THREADID threadid, const char* file, int line, bool start) {
  if (file == NULL) return;
  struct AFTask* cur_step_parent = cur_dpst_index[threadid].top();
  if (start) {
    cur_step_parent->start.line = line;
    delete cur_step_parent->start.filename;
    cur_step_parent->start.filename = new char[64];
    strcpy(cur_step_parent->start.filename, file);
  } else {
    cur_step_parent->end.line = line;
    delete cur_step_parent->end.filename;
    cur_step_parent->end.filename = new char[64];
    strcpy(cur_step_parent->end.filename, file);
  }
}
