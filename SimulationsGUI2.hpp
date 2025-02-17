#pragma once
#include <SFML/Graphics.hpp>													//[7]
#include <TGUI/TGUI.hpp>            							//[11]
#include "simulation2.hpp"
#include <sstream>



// Klasse SimulationsGUI für das GUI-Fenster zur Einstellung und Durchführung der Simulation
class SimulationsGUI {
private:
    sf::RenderWindow window; 												//[6]
    tgui::GuiSFML gui;															//[11]
    bool saisonaleEinfluesse;
    double mittelwert;
    double standardabweichung;
    bool alleEingabenValid;
    std::stringstream logStream;


    //Konstanten für die Simulation
    const double meanProdukteProAuftrag = 10.0;
    const double stdProdukteProAuftrag = 1.0;
    const float SAISON_BASIS_AMPLITUDE = 0.15f;  // 15% vom Mittelwert
    const float frequenz = 2.0f * 3.14159f / 365.0f;


    // Validiert die Eingabe für den Mittelwert und aktualisiert die Hintergrundfarbe des Eingabefeldes
    void validiereMittelwert(tgui::EditBox::Ptr mean_Edit);

    // Validiert die Eingabe für die Standardabweichung und überprüft, ob sie im gültigen Bereich liegt
    void validiereStandardabweichung(tgui::EditBox::Ptr SD_edit, tgui::EditBox::Ptr mean_Edit);

    // Startet die Simulation im Hintergrund-Thread, um die GUI-Interaktion nicht zu blockieren
    void starteSimulationImHintergrund();

    // Initialisiert alle GUI-Elemente und ihre Positionen auf dem Fenster
    void initialisiereBenutzerInterface();

    // Überprüft die Gültigkeit aller Simulationsparameter, um eine gültige Simulation zu ermöglichen
    void validiereSimulationsParameter();


    // Hilfsfunktion für das Logging/Dokumentieren, um Nachrichten sowohl in die Konsole als auch in den internen Stream/Textdatei zu schreiben
    template<typename T>
    void log(const T& message) {
        std::cout << message;
        logStream << message;
    }

    // Fügt eine neue Zeile im Log hinzu, um die Lesbarkeit zu verbessern
    template<typename T>
    void logLine(const T& message) {
        log(message);
        log('\n');
    }


    // Startet die Simulation basierend auf den vom Benutzer eingegebenen Parametern
    void starteSimulation();




public:
    // Konstruktor: Initialisiert das Fenster und die GUI-Parameter
    SimulationsGUI();

    // Startet das GUI-Fenster und wartet auf Benutzereingaben
    void starteSimulationsGUI();
};
