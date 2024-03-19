#ifndef _SCHEDULING_ALGORITHM_RMA_H
#define _SCHEDULING_ALGORITHM_RMA_H

#include <vector>

#include "SchedulingAlgorithm.h"

class SchedulingAlgorithmRMA : public SchedulingAlgorithm
{

private:

public:

  // Determines the highest-priority task in a set.
  Task *GetHighestPriorityTask(std::vector<Task> &tasks);

};

#endif