#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include <vector>

#include "Task.h"
#include "SchedulingAlgorithm.h"

class Schedule
{
private:

  bool openedFile;
  
  // All the tasks that are to be scheduled.
  std::vector<Task> tasks;

  // The task that is currently running.
  Task *runningTask;

  // The time that the schedule lasts for.
  int duration;

  // The current time that the scheduler is simulating.
  int currentTime;

  // Gets the schedule's current active task.
  Task *GetRunningTask();

  // Switches from a running task to a non-running task.
  void SwitchTask(Task *from, Task *to);

public:
  Schedule(const char* inputFilePath);
  ~Schedule();

  bool GetOpenedFile();

  // Runs the task schedule with the given scheduling algorithm.
  void Run(SchedulingAlgorithm &schedulingAlgorithm);

  int GetCurrentTime() const;
};

#endif