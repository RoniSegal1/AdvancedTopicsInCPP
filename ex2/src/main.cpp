#include "GameManager.h"
#include "MyPlayerFactory.h"
#include "MyTankAlgorithmFactory.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: tanks_game <input_file>" << std::endl;
        return 1;
    }

    try {
        GameManager game(std::make_unique<MyPlayerFactory>(), std::make_unique<MyTankAlgorithmFactory>());
        std::cerr << "Starting to read board" << std::endl;
        game.readBoard(argv[1]);
        std::cerr << "Finished reading board" << std::endl;

        std::cerr << "Running game" << std::endl;
        game.run();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}
