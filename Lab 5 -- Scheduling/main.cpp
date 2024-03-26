#include <iostream>
#include <fstream>
#include <string>

#include "Task.h"
#include "SchedulingAlgorithm.h"
#include "SchedulingAlgorithmRMA.h"
#include "Schedule.h"
#include "Logger.h"

const std::string helpMessage = "Usage: ./scheduler input.txt output.txt";
const std::string fileErrorMessage = "Failed to open ";
const int argumentCount = 2;  // Doesn't count the program name.

int main(int argc, char *argv[]) {

  // Check correct number of input arguments.
  if (argc != (argumentCount + 1)) {
    std::cout << helpMessage;
    return 0;
  }

  Schedule schedule = Schedule(argv[1]);
  if (!schedule.GetOpenedFile()) {
    std::cout << fileErrorMessage << argv[1] << std::endl;
    return 0;
  }

  SchedulingAlgorithmRMA rma;

  // Attempt to open output file.
  Logger logger(argv[2], &schedule, &rma);
  if (!logger.GetFileOpened()) {
    std::cout << fileErrorMessage << argv[2] << std::endl;
    return 0;
  }

  schedule.Run(rma);

  // Run EDF simulation.

  // Run LLF simulation.

  // Close everything down.

  return 0;
}