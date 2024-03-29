#ifndef _TASK_H
#define _TASK_H

class Task
{
private:

  // The unique ID of the task. Uniqueness in not enforced by any code, so be
  // careful.
  char taskId;

  // Whether the task is periodic.
  bool isPeriodic;

  // The period of the task, if the task is periodic. Undefined if the task
  // is aperiodic.
  int period;

  // The time that the task will take to execute
  int executionTime;

  // The amount of time between the beginning of the simulation and the task's
  // first launch.
  int launchOffset;

  // Whether this task is allowed to run.
  bool isEnabled;

  // Whether this task has launched at any point in the past.
  bool hasLaunched;

  // Whether this task is currently running.
  bool isRunning;

  // The amount of work left to do on this task.
  int workLeft;

  // The amount of time until this task is launched for this first time.
  int launchRelative;

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

  static const int highestPossiblePeriod = 10000;

  Task(
    char taskId, 
    bool isPeriodic,
    int launchOffset, 
    int period, 
    int executionTime
  );
  ~Task();

  // Checks the task's timing information and updates its state (finished,
  // running, etc.) accordingly.
  void Check();

  // Moves the task forward in time by one millisecond. 
  void Tick();

  // Resets the task to its initial state, so it can be used for another 
  // simulation.
  void Reset();

  // Gets the unique task ID.
  char GetTaskId() const;

  // Gets whether the task is enabled.
  bool GetEnabled() const;

  // Gets whether the task is ready to run.
  bool GetReady() const;

  // Gets whether the task has finished.
  bool GetFinished() const;

  // Gets whether the task is currently running.
  bool GetRunning() const;

  // Sets whether the task is currently running.
  void SetRunning(bool isRunning);

  // Gets whether the task is periodic.
  bool GetPeriodic() const;

  // Gets the period of the task.
  int GetPeriod() const;

  int GetRelativeDeadline() const;

  int GetWorkLeft() const;

};

#endif