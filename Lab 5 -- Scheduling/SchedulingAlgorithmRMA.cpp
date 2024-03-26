#include <vector>

#include "Task.h"
#include "SchedulingAlgorithmRMA.h"

const char humanReadableName[] = "Rate-Monotonic Analysis (RMA)";

Task *SchedulingAlgorithmRMA::GetHighestPriorityTask(std::vector<Task> &tasks) const {

  // Find the ready task that has the shortest period.
  int shortestPeriod = Task::highestPossiblePeriod;
  Task *shortestPeriodTask = nullptr;
  for (Task &task : tasks) {
    if (
      task.GetPeriodic()                &&
      task.GetEnabled()                 && 
      !task.GetFinished()               &&
      task.GetPeriod() < shortestPeriod 
    ) {
      shortestPeriod = task.GetPeriod();
      shortestPeriodTask = &task;
    }
  }
  if (shortestPeriodTask != nullptr) {
    return shortestPeriodTask;
  }

  // If no periodic task was found, look through the aperiodic tasks.
  int soonestDeadline = Task::highestPossiblePeriod;
  Task *soonestDeadlineTask = nullptr;
  for (Task &task : tasks) {
    if (
      !task.GetPeriodic()                           &&
      task.GetEnabled()                             &&
      !task.GetFinished()                           &&
      task.GetRelativeDeadline() < soonestDeadline
    ) {
      soonestDeadline = task.GetRelativeDeadline();
      soonestDeadlineTask = &task;
    }
  }
  if (soonestDeadlineTask != nullptr) {
    return soonestDeadlineTask;
  }

  return nullptr;

}

const char *SchedulingAlgorithmRMA::GetHumanReadableName() const {
  return humanReadableName;
}