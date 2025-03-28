#include "SimulationsGUI2.hpp"
#include <iostream>
#include <stdexcept>

// Function to handle program initialization and error management
void runSimulation() {
    SimulationsGUI Simulation;

    // Attempt to start the simulation with more descriptive error handling
    try {
        Simulation.starteSimulationsGUI();
    }
    catch (const std::runtime_error& re) {
        std::cerr << "Runtime Error: " << re.what() << std::endl;
        throw;
    }
    catch (const std::logic_error& le) {
        std::cerr << "Logic Error: " << le.what() << std::endl;
        throw;
    }
}

int main() {
    try {
        // Call dedicated function for simulation to improve code structure
        runSimulation();
    }
    catch (const std::exception& e) {
        std::cerr << "Schwerwiegender Fehler: " << e.what() << std::endl;

        // Add a bit more contextual information
        std::cerr << "Die Simulation konnte nicht gestartet werden." << std::endl;
        return EXIT_FAILURE;
    }

    // More natural way of waiting for user input
    std::cout << "\nProgramm abgeschlossen. Drücken Sie Enter zum Beenden..." << std::endl;
    std::cin.ignore();
    std::cin.get();

    return EXIT_SUCCESS;
}
