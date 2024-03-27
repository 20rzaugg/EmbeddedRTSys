#ifndef _SCHEDULING_ALGORITHM_EDF_H
#define _SCHEDULING_ALGORITHM_EDF_H

#include <vector>

#include "SchedulingAlgorithm.h"

class SchedulingAlgorithmEDF : public SchedulingAlgorithm
{

private:

public:

  // Determines the highest-priority task in a set.
  Task *GetHighestPriorityTask(std::vector<Task> &tasks) const;

  // Gets a human-readable name for the scheduling algorithm.
  const char *GetHumanReadableName() const;

};

#endif