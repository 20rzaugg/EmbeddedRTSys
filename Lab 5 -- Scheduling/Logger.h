#ifndef _LOGGER_H
#define _LOGGER_H

#include <cstdio>

#include "Schedule.h"
#include "SchedulingAlgorithm.h"
#include "Task.h"

class Logger
{

private:
  const Schedule            *schedule;
  const SchedulingAlgorithm *schedulingAlgorithm;
  FILE                *logFile;

  void PrintTimestamp() const;

  static Logger *instance;

public:

  Logger(
    const char                *logFilePath,
    const Schedule            *schedule,
    const SchedulingAlgorithm *schedulingAlgorithm
  );

  ~Logger();

  static Logger *Instance(); 

  // Gets whether the log file was opened successfully.
  bool GetFileOpened() const;

  // Logs that a task run is beginning.
  void LogRunBeginning() const;

  // Logs a task switch.
  void LogTaskSwitch(
    const Task *from,
    const Task *to
  ) const;

  void LogMissedDeadline(
    const Task &task
  ) const;

  void LogFinishedTask(
    const Task &task
  ) const;

  void LogRunEnding() const;

  void LogTickBreak() const;

};

#endif
