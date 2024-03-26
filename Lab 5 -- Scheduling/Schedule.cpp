#include "Schedule.h"

#include <stdexcept>
#include <iostream>

#include "SchedulingAlgorithm.h"

Schedule::Schedule() {

  Task task = Task('A', true, 4, 3);
  this->tasks.push_back(task);

  this->duration = 5;

}

Schedule::~Schedule() {

}

void Schedule::Run(SchedulingAlgorithm &schedulingAlgorithm) {

  // Initialize absolute time.
  int currentTime = 0;

  // Loop until simulation is over.
  for (int currentTime = 0; currentTime < duration; currentTime++)
  {

    // Determine if the current task needs to be switched.
    Task *runningTask = GetRunningTask();
    Task *highestPriorityTask = schedulingAlgorithm.GetHighestPriorityTask(this->tasks);
    SwitchTask(runningTask, highestPriorityTask);

    // Process each task.
    for (Task &task : tasks) {
      task.Tick();
    }
  }
  
}

Task *Schedule::GetRunningTask() {

  Task *runningTask = nullptr;
  for (Task &task : tasks) {
    if (task.GetRunning()) {
      if (runningTask == nullptr) {
        runningTask = &task;
      } else {
        throw std::runtime_error("Schedule has more than one running task.");
      }
    }
  }

  return runningTask;

}

void Schedule::SwitchTask(Task *from, Task *to) {

  if (from != nullptr) {
    from->SetRunning(false);
  }

  if (to != nullptr) {
    to->SetRunning(true);
  }

  // TODO: Report the task switch.

}