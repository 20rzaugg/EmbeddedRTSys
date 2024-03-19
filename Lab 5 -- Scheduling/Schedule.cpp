#include "Schedule.h"

void Schedule::Run() {

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
    // Do this by calling out to a seperate "SchedulingAlgorithm" object.

  }
  
}