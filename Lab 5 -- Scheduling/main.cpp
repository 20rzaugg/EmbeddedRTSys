#include <iostream>
#include <fstream>
#include <string>

#include "Task.h"
#include "SchedulingAlgorithm.h"
#include "SchedulingAlgorithmRMA.h"
#include "SchedulingAlgorithmEDF.h"
#include "SchedulingAlgorithmLLF.h"
#include "Schedule.h"
#include "Logger.h"

const std::string helpMessage = "Usage: ./scheduler input.txt output.txt";
const std::string fileErrorMessage = "Failed to open ";
const int argumentCount = 2;  // Doesn't count the program name.

//#define ALGORITHM_RMA
//#define ALGORITHM_EDF
#define ALGORITHM_LLF

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

  #ifdef ALGORITHM_RMA
    SchedulingAlgorithmRMA rma;

    // Attempt to open output file.
    Logger logger(argv[2], &schedule, &rma);
    if (!logger.GetFileOpened()) {
      std::cout << fileErrorMessage << argv[2] << std::endl;
      return 0;
    }

    schedule.Run(rma);
  #endif

  #ifdef ALGORITHM_EDF
    SchedulingAlgorithmEDF edf;

    // Attempt to open output file.
    Logger logger(argv[2], &schedule, &edf);
    if (!logger.GetFileOpened()) {
      std::cout << fileErrorMessage << argv[2] << std::endl;
      return 0;
    }

    schedule.Run(edf);

  #endif

  #ifdef ALGORITHM_LLF
    
    SchedulingAlgorithmLLF llf;

    // Attempt to open output file.
    Logger logger(argv[2], &schedule, &llf);
    if (!logger.GetFileOpened()) {
      std::cout << fileErrorMessage << argv[2] << std::endl;
      return 0;
    }

    schedule.Run(llf);
  
  #endif
  return 0;
}