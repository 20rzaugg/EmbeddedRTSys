#include "Schedule.h"

#include <stdexcept>
#include <iostream>

#include "SchedulingAlgorithm.h"

Schedule::Schedule() {

}

Schedule::~Schedule() {

}

void Schedule::Run(SchedulingAlgorithm &schedulingAlgorithm) {

  // Initialize absolute time.
  int currentTime = 0;

  // Loop until simulation is over.
  for (int currentTime = 0; currentTime < duration; currentTime++)
  {
    
    // Process each task.
    for (Task task : tasks) {
      task.Tick();
    }

    // Determine if the current task needs to be switched.
    Task *runningTask= GetRunningTask();
    Task *highestPriorityTask = schedulingAlgorithm.GetHighestPriorityTask(this->tasks);
    if (
      runningTask               != nullptr                  &&
      highestPriorityTask       != nullptr                  &&
      runningTask->GetTaskId()  == runningTask->GetTaskId()
    ) {
      
    } else {
      // The currently-running tasks needs to be preempted.
      Preempt(runningTask, highestPriorityTask);
    }

  }
  
}

Task *Schedule::GetRunningTask() {

  Task *runningTask = nullptr;
  for (Task task : tasks) {
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

void Schedule::Preempt(Task *preempted, Task *preemptor) {

  if (preempted != nullptr) {
    preempted->SetRunning(false);
  }
  if (preemptor != nullptr) {
    preemptor->SetRunning(true);
  }
  // Log the preemption.

}