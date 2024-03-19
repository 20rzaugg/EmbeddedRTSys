#ifndef _TASK_H
#define _TASK_H

class Task
{
private:

  // Whether the task is periodic.
  bool isPeriodic;

  // The period of the task, if the task is periodic. Undefined if the task
  // is aperiodic.
  int period;

  // The time that the task will take to execute
  int executionTime;

  // Whether this task is allowed to run.
  int isEnabled;

  // Whether this task is currently running.
  int isRunning;

  // The amount of work left to do on this task.
  int workLeft;

  // The time remaining until the task's next deadline.
  int nextDeadlineRelative;

  // Disables the task. It will no longer do anything at all, including logging
  // missed deadlines.
  void Disable();

  // Launches the task.
  void Launch();

  // Sets the task's state as it finishes its work.
  void Finish();

  // Called whenever a task reaches its deadline.
  void OnDeadline();

  // Renews the tasks's deadline.
  void RenewDeadline();
  
public:

  Task::Task(bool isPeriodic, int period, int executionTime);
  ~Task();

  // Moves the task forward in time by one millisecond. Updates everything
  // about the task's state.
  void Tick();

  // Gets whether the task is enabled.
  bool GetEnabled();

  // Gets whether the task is ready to run.
  bool GetReady();

  // Gets whether the task has finished.
  bool GetFinished();

  // Gets whether the task is currently running.
  bool GetRunning();

  // Sets whether the task is currently running.
  bool SetRunning(bool isRunning);

};

#endif