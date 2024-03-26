#include <vector>

#include "Task.h"
#include "SchedulingAlgorithmRMA.h"

Task *SchedulingAlgorithmRMA::GetHighestPriorityTask(std::vector<Task> &tasks) {

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