#include <iostream>
#include <fstream>
#include <string>
#include "GameManager.h"

int main(int argc, char* argv[]) {
    // Check if exactly one argument was provided (the input file)
    if (argc != 2) {
        std::cerr << "Usage: tanks_game <input_file>" << std::endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    std::ifstream inputFile(inputFileName);
    // Verify input file can be opened
    if (!inputFile.is_open()) {
        std::cerr << "Error: Cannot open input file: " << inputFileName << std::endl;
        return 1;
    }

    // Create output file name based on input
    std::string outputFileName = "output_" + inputFileName;
    std::ofstream outputFile(outputFileName);
    // Verify output file can be created
    if (!outputFile.is_open()) {
        std::cerr << "Error: Cannot create output file: " << outputFileName << std::endl;
        return 1;
    }

    // Create and run the game
    GameManager gameManager;

    // Set the output stream (for bad steps etc.)
    gameManager.setOutputLog(outputFile);

    // Load the game from input file
    if (!gameManager.loadGame(inputFileName)) {
        // input_errors.txt will be written internally if needed
        return 1;
    }

    // Start the simulation
    gameManager.run();

    return 0;
}
