#include "Task.h"

Task::Task(char taskId, bool isPeriodic, int period, int executionTime) {
  this->taskId        = taskId;
  this->isPeriodic    = isPeriodic;
  this->period        = period;
  this->executionTime = executionTime;

  this->isEnabled             = true;
  this->isRunning             = false;
  this->workLeft              = executionTime;
  this->nextDeadlineRelative  = period;
}

Task::~Task() {
  
}

void Task::Tick() {

  if (!isEnabled) {
    return;
  }

  // The deadline draws nearer.
  nextDeadlineRelative = nextDeadlineRelative - 1;

  // If the task is running and not yet done,
  if (GetRunning() && !GetFinished()) {
    workLeft = workLeft - 1;
    // If the task just finished,
    if (GetFinished()) {
      Finish();
    }
  }

  // If the task's deadline is up,
  if (nextDeadlineRelative <= 0) {
    OnDeadline();
  }

}

char Task::GetTaskId() {
  return this->taskId;
}

bool Task::GetEnabled() {
  return isEnabled;
}

bool Task::GetReady() {
  return isEnabled && workLeft > 0;
}

bool Task::GetFinished() {
  return isEnabled && workLeft <= 0;
}

bool Task::GetRunning() {
  return isEnabled && isRunning;
}

void Task::SetRunning(bool isRunning) {
  this->isRunning = this->isEnabled && isRunning;
}

int Task::GetPeriod() {
  return this->period;
}

void Task::Disable() {
  isEnabled = false;
}

void Task::Launch() {
  workLeft = executionTime;
}

void Task::Finish() {
  isRunning = false;
}

void Task::OnDeadline() {

  if (workLeft > 0) {
    Disable();
    // Log missed deadline.
  } else {
    if (isPeriodic) {
      RenewDeadline();
      Launch();
    } else {
      // Aperiodic tasks only run once.
      Disable();
    }
  }

}

void Task::RenewDeadline() {
  nextDeadlineRelative = period;
}