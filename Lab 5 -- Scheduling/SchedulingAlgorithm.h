#ifndef _SCHEDULING_ALGORITHM_H
#define _SCHEDULING_ALGORITHM_H

#include <vector>

#include "Task.h"

// An interface that different scheduling algorithms can implement.

class SchedulingAlgorithm
{

private:
  // Interface does not define any private members.

public:
  
  // Determines the highest-priority task in a set.
  virtual Task *GetHighestPriorityTask(std::vector<Task> &tasks) const = 0;

  // Gets a human-readable name for the scheduling algorithm.
  virtual const char *GetHumanReadableName() const = 0;

};

#endif