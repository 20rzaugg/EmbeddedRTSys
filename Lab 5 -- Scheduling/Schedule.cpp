#include "Schedule.h"

#include <stdexcept>
#include <iostream>
#include <cstdio>

#include "SchedulingAlgorithm.h"
#include "Logger.h"

Schedule::Schedule(const char *inputFilePath) {

  FILE *inputFile = fopen(inputFilePath, "r");
  if (inputFile == nullptr) {
    this->openedFile = false;
    return;
  }
  this->openedFile = true;

  // Scan in first two lines of input file (simulation duration and task
  // count).
  int periodicTaskCount;
  fscanf(inputFile, " %d ", &periodicTaskCount);
  fscanf(inputFile, " %d ", &(this->duration));

  // Creates each periodic task.
  for (int i = 0; i < periodicTaskCount; i++) {
    char taskId;
    int taskExecutionTime;
    int taskPeriod;
    fscanf(inputFile, "%c , %d , %d ", &taskId, &taskExecutionTime, &taskPeriod);
    Task task = Task(taskId, true, 0, taskPeriod, taskExecutionTime);
    this->tasks.push_back(task);
  }

  // Creates aperiodic tasks.
  int aperiodicTaskCount;
  fscanf(inputFile, " %d ", &aperiodicTaskCount);
  for (int i = 0; i < aperiodicTaskCount; i++) {
    char taskId;
    int taskExecutionTime;
    int taskLaunchOffset;
    fscanf(inputFile, "%c, %d, %d ", &taskId, &taskExecutionTime, &taskLaunchOffset);
    Task task = Task(
      taskId, 
      false, 
      taskLaunchOffset, 
      500 - taskLaunchOffset, 
      taskExecutionTime
    );
    this->tasks.push_back(task);
  }

  fclose(inputFile);

}

Schedule::~Schedule() {

}

void Schedule::Run(SchedulingAlgorithm &schedulingAlgorithm) {

  Logger::Instance()->LogRunBeginning();
  Logger::Instance()->LogTickBreak();

  this->runningTask = nullptr;

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

bool Schedule::GetOpenedFile() {
  return this->openedFile;
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
