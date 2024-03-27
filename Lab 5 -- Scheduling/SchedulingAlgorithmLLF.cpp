#include <vector>

#include "Task.h"
#include "SchedulingAlgorithmLLF.h"

const char humanReadableName[] = "Least Laxity First (LLF)";

Task *SchedulingAlgorithmLLF::GetHighestPriorityTask(std::vector<Task> &tasks) const {

  // Find the ready task that has the shortest period.
  int leastLaxity = Task::highestPossiblePeriod;
  Task *leastLaxityTask = nullptr;
  for (Task &task : tasks) {
    if (
      task.GetEnabled()                 && 
      !task.GetFinished()               &&
      task.GetRelativeDeadline() - task.GetWorkLeft() < leastLaxity
    ) {
      leastLaxity = task.GetRelativeDeadline() - task.GetWorkLeft();
      leastLaxityTask = &task;
    }
  }
  return leastLaxityTask;
}

const char *SchedulingAlgorithmLLF::GetHumanReadableName() const {
  return humanReadableName;
}
