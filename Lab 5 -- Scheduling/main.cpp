#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const std::string helpMessage = "Usage: ./scheduler input.txt output.txt";
const std::string fileErrorMessage = "Failed to open ";
const int argumentCount = 2;  // Doesn't count the program name.

int main(int argc, char *argv[]) {

  // Check correct number of input arguments.
  if (argc != (argumentCount + 1)) {
    std::cout << helpMessage;
    return 0;
  }

  // Attempt to open input file.
  std::ifstream inputFileStream;
  inputFileStream.open(argv[1]);
  if (!inputFileStream.is_open()) {
    std::cout << fileErrorMessage << argv[1] << std::endl;
    return 0;
  }

  // Attempt to open output file.
  std::ofstream outputFileStream;
  outputFileStream.open("output.txt");
  if (!outputFileStream.is_open()) {
    std::cout << fileErrorMessage << argv[2] << std::endl;
    return 0;
  }

  // Construct schedule object from file.
  inputFileStream.close();

  // Run RM simulation.

  // Run EDF simulation.

  // Run LLF simulation.

  // Close everything down.
  outputFileStream << "Hello World." << std::endl;
  outputFileStream.close();

  return 0;
}