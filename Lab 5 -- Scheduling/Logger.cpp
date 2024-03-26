#include "Logger.h"

#include <cstdio>

#include "Schedule.h"
#include "SchedulingAlgorithm.h"
#include "Task.h"

Logger *Logger::instance;

Logger::Logger(
  const char                *logFilePath,
  const Schedule            *schedule,
  const SchedulingAlgorithm *schedulingAlgorithm
) {

  if (instance == nullptr) {
    instance = this;
  } else {
    // Error handling?
  }

  this->schedule            = schedule;
  this->schedulingAlgorithm = schedulingAlgorithm;

  this->logFile = fopen(logFilePath, "w");
}

Logger::~Logger() {
  fclose(this->logFile);
}

Logger *Logger::Instance() {
  return Logger::instance;
}

bool Logger::GetFileOpened() const {
  return (this->logFile != NULL);
}

void Logger::LogRunBeginning() const {
  fprintf(
    this->logFile,
    "##### NEW RUN #####\nBeginning run with %s algorithm.\n",
    this->schedulingAlgorithm->GetHumanReadableName()
  );
}

// Logs a task being launched.
void Logger::LogTaskLaunch(const Task *task) const {
  PrintTimestamp();
  fprintf(
    logFile,
    "Launching Task %c (Deadline in %d ticks, %d work to do).\n",
    task->GetTaskId(),
    task->GetRelativeDeadline(),
    task->GetWorkLeft()
  );
}

void Logger::LogTaskSwitch(
  const Task *from,
  const Task *to
) const {
  
  // If the two tasks are the same, do nothing because no switch was really
  // made.
  if (from == to) {
    return;
  }

  this->PrintTimestamp();

  // If the from task was null, the schedule is switching out of the null task.
  if (from == nullptr) {
    fprintf(
      this->logFile,
      "Switching from idle task to Task %c.\n",
      to->GetTaskId()
    );
    return;
  }

  // If the to task is null, the schedule is switching into the null task.
  else if (to == nullptr) {
    fprintf(
      this->logFile,
      "Switching from Task %c to idle task.\n",
      from->GetTaskId()
    );
    return;
  }

  // If both tasks are non-null, two proper tasks are being switched between.
  else {
    fprintf(
      this->logFile,
      "Switching from Task %c to Task %c.\n",
      from->GetTaskId(),
      to->GetTaskId()
    );
    return;
  }

}

void Logger::LogMissedDeadline(const Task &task) const {

  this->PrintTimestamp();
  fprintf(
    logFile,
    "Task %c missed its deadline. It will no longer be run or scheduled.\n",
    task.GetTaskId()
  );

}

void Logger::LogFinishedTask(const Task &task) const {

  this->PrintTimestamp();
  fprintf(
    this->logFile,
    "Task %c finished execution ahead of its deadline.\n",
    task.GetTaskId()
  );

}

void Logger::LogRunEnding() const {
  fprintf(
    this->logFile,
    "Simulation has ended. Goodbye!\n"
  );
}

void Logger::LogTickBreak() const {
  fprintf(
    this->logFile,
    "\n"
  );
}

void Logger::PrintTimestamp() const {

  fprintf(
    logFile,
    "[time=%d]\t",
    this->schedule->GetCurrentTime()
  );

}