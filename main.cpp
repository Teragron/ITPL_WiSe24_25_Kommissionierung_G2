#include "SimulationsGUI2.hpp"
#include <iostream>
int main() 
{




        SimulationsGUI Simulation;
        Simulation.starteSimulationsGUI();   // Starte Simulation, beginnend mit der GUI
    


    std::cout << "\nDruecken Sie die Enter-Taste, um das Programm zu beenden..." << std::endl;
    std::cin.get();
    return 0;
}