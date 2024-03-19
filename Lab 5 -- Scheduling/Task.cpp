#include "Task.h"

Task::Task(bool isPeriodic, int period, int executionTime) {
  this->isPeriodic = isPeriodic;
  this->period = period;
  this->executionTime = executionTime;
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

bool Task::SetRunning(bool isRunning) {
  this->isRunning = isEnabled && isRunning;
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