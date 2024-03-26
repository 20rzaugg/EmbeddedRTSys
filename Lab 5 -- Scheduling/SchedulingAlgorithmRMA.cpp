#include <vector>

#include "Task.h"
#include "SchedulingAlgorithmRMA.h"

const char humanReadableName[] = "Rate-Monotonic Analysis (RMA)";

Task *SchedulingAlgorithmRMA::GetHighestPriorityTask(std::vector<Task> &tasks) const {

  // Find the ready task that has the shortest period.
  int shortestPeriod = Task::highestPossiblePeriod;
  Task *shortestPeriodTask = nullptr;
  for (Task &task : tasks) {
    if (task.GetPeriod() < shortestPeriod && task.GetEnabled() && !task.GetFinished()) {
      shortestPeriod = task.GetPeriod();
      shortestPeriodTask = &task;
    }
  }

  return shortestPeriodTask;

}

const char *SchedulingAlgorithmRMA::GetHumanReadableName() const {
  return humanReadableName;
}