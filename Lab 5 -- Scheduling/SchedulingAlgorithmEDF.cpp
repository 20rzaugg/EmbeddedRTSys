#include <vector>

#include "Task.h"
#include "SchedulingAlgorithmEDF.h"

const char humanReadableName[] = "Earliest Deadline First (EDF)";

Task *SchedulingAlgorithmEDF::GetHighestPriorityTask(std::vector<Task> &tasks) const {

  // Find the ready task that has the shortest period.
  int earliestDeadline = Task::highestPossiblePeriod;
  Task *earliestDeadlineTask = nullptr;
  for (Task &task : tasks) {
    if (
      task.GetEnabled()                 && 
      !task.GetFinished()               &&
      task.GetRelativeDeadline() < earliestDeadline 
    ) {
      earliestDeadline = task.GetRelativeDeadline();
      earliestDeadlineTask = &task;
    }
  }
  return earliestDeadlineTask;
}

const char *SchedulingAlgorithmEDF::GetHumanReadableName() const {
  return humanReadableName;
}