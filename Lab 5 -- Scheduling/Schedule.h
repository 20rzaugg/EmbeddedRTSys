#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include <vector>

#include "Task.h"

class Schedule
{
private:
  
  // All the tasks that are to be scheduled.
  std::vector<Task> tasks;

  // The time that the schedule lasts for.
  int duration;

public:
  Schedule(/* args */);
  ~Schedule();

  // Runs the task schedule.
  void Run();
};

Schedule::Schedule(/* args */)
{
}

Schedule::~Schedule()
{
}

#endif