#include "Schedule.h"

#include <stdexcept>
#include <iostream>

#include "SchedulingAlgorithm.h"
#include "Logger.h"

Schedule::Schedule() {

  Task task = Task('A', true, 4, 3);
  this->tasks.push_back(task);

  task = Task('B', true, 5, 2);
  this->tasks.push_back(task);

  this->duration = 10;

  this->runningTask = nullptr;

}

Schedule::~Schedule() {

}

void Schedule::Run(SchedulingAlgorithm &schedulingAlgorithm) {

  Logger::Instance()->LogRunBeginning();
  Logger::Instance()->LogTickBreak();

  // Loop until simulation is over.
  for (
    this->currentTime = 0; 
    this->currentTime < this->duration; 
    this->currentTime++
  ) {

    for (Task &task : tasks) {
      task.Check();
    }

    // Determine if the current task needs to be switched.
    Task *highestPriorityTask = schedulingAlgorithm.GetHighestPriorityTask(this->tasks);
    SwitchTask(this->runningTask, highestPriorityTask);

    // Process each task.
    for (Task &task : tasks) {
      task.Tick();
    }

  }

  Logger::Instance()->LogRunEnding();
  
}

int Schedule::GetCurrentTime() const {
  return this->currentTime;
}

Task *Schedule::GetRunningTask() {
  return this->runningTask;
}

void Schedule::SwitchTask(Task *from, Task *to) {

  if (from != nullptr) {
    from->SetRunning(false);
  }

  if (to != nullptr) {
    to->SetRunning(true);
  }

  this->runningTask = to;

  Logger::Instance()->LogTaskSwitch(from, to);

}
