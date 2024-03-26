#include "Task.h"

#include "Logger.h"

Task::Task(char taskId, bool isPeriodic, int period, int executionTime) {
  this->taskId        = taskId;
  this->isPeriodic    = isPeriodic;
  this->period        = period;
  this->executionTime = executionTime;

  this->isEnabled             = true;
  this->isRunning             = false;
  this->workLeft              = 0;
  this->nextDeadlineRelative  = 0;
}

Task::~Task() {
  
}

// Checks the task's timing information and updates its state (finished,
// running, etc.) accordingly.
void Task::Check() {
  if (!isEnabled) {
    return;
  }

  if (GetRunning() && GetFinished()) {
    Finish();
  }

  if (nextDeadlineRelative <= 0) {
    OnDeadline();
  }

}

void Task::Tick() {

  if (!isEnabled) {
    return;
  }

  // If the task is running and not yet done,
  if (GetRunning() && !GetFinished()) {
    workLeft = workLeft - 1;
  }

  if (nextDeadlineRelative > 0) {
    // The deadline draws nearer.
    nextDeadlineRelative = nextDeadlineRelative - 1;
  }

}

char Task::GetTaskId() const {
  return this->taskId;
}

bool Task::GetEnabled() const {
  return isEnabled;
}

bool Task::GetReady() const {
  return isEnabled && workLeft > 0;
}

bool Task::GetFinished() const {
  return isEnabled && workLeft <= 0;
}

bool Task::GetRunning() const {
  return isEnabled && isRunning;
}

void Task::SetRunning(bool isRunning) {
  this->isRunning = this->isEnabled && isRunning;
}

int Task::GetPeriod() const {
  return this->period;
}

int Task::GetRelativeDeadline() const {
  return this->nextDeadlineRelative;
}

int Task::GetWorkLeft() const {
  return this->workLeft;
}

void Task::Disable() {
  isEnabled = false;
}

void Task::Launch() {
  workLeft = executionTime;
  RenewDeadline();
  Logger::Instance()->LogTaskLaunch(this);
}

void Task::Finish() {
  isRunning = false;
  Logger::Instance()->LogFinishedTask(*this);
}

void Task::OnDeadline() {

  if (workLeft > 0) {
    Disable();
    Logger::Instance()->LogMissedDeadline(*this);
  } else {
    if (isPeriodic) {
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