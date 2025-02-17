#include "SimulationsGUI2.hpp"
#include <iostream>
int main() {
    try {                                                                                               // [1] 
        SimulationsGUI Simulation;
        Simulation.starteSimulationsGUI();                                      // Starte Simulation, beginnend mit der GUI
    }
    catch (const std::exception& e) {                                                                       // [1], [3]
        std::cerr << "Fehler: " << e.what() << std::endl;                                                   // [2] 
        return 1;
    }
    catch (...) {
        std::cerr << "Unbekannter Fehler aufgetreten!" << std::endl;
        return 1;
    }
    return 0;
}
