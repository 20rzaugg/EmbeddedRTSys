#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include <vector>

#include "Task.h"
#include "SchedulingAlgorithm.h"

class Schedule
{
private:
  
  // All the tasks that are to be scheduled.
  std::vector<Task> tasks;

  // The time that the schedule lasts for.
  int duration;

  // Gets the schedule's current active task.
  Task *GetRunningTask();

  // Preempts a running task with another non-running task.
  void Preempt(Task *preempted, Task *preemptor);

public:
  Schedule();
  ~Schedule();

  // Runs the task schedule with the given scheduling algorithm.
  void Run(SchedulingAlgorithm &schedulingAlgorithm);
};

#endif