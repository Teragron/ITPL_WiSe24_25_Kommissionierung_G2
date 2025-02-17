#include "SimulationsGUI2.hpp"
#include <future>					                                                                                        //[3]
#include "pbPlots.hpp"				                                                                                                //[10]
#include "supportLib.hpp"				                                                                                        //[10]
#include <fstream>					                                                                                        //[3]


// Konstruktor für das GUI-Fenster zur Simulationseinstellung
SimulationsGUI::SimulationsGUI()
    : window(sf::VideoMode(400, 300), "Simulationsparameter", sf::Style::Titlebar | sf::Style::Close)			                        //[6]
    , gui(window)
    , saisonaleEinfluesse(false)
    , mittelwert(0)
    , standardabweichung(0)
    , alleEingabenValid(false)
{
    initialisiereBenutzerInterface();                  // Initialisiert das GUI-Interface                           
}




// Validiert den Mittelwert aus der Eingabe
void SimulationsGUI::validiereMittelwert(tgui::EditBox::Ptr mean_Edit) {
    try {
        double wert = std::stod(mean_Edit->getText().toStdString());		                                                                //[3]
        // Prüft, ob der Wert im zulässigen Bereich liegt
        if (wert >= 1 && wert <= 3000) {
            mittelwert = wert;
            mean_Edit->getRenderer()->setBackgroundColor(tgui::Color::White);

            // Überprüft andere Eingabefelder, um sicherzustellen, dass alle Eingaben korrekt sind
            for (auto& widget : gui.getWidgets()) {
                auto editBox = std::dynamic_pointer_cast<tgui::EditBox>(widget);
                if (editBox && editBox != mean_Edit && !editBox->getText().empty()) {
                    
                    if (editBox->getPosition().y == 150) {  // Prüft speziell die Standardabweichungs-Box
                        validiereStandardabweichung(editBox, mean_Edit);
                    }
                }
            }
        }
        else {
            // Falls ungültig, setzt den Wert zurück und markiert das Feld rot
            mittelwert = 0;
            mean_Edit->getRenderer()->setBackgroundColor(tgui::Color::Red);
            alleEingabenValid = false;
        }
    }
    catch (...) {
        mittelwert = 0; // Bei Fehler Wert zurücksetzen
        mean_Edit->getRenderer()->setBackgroundColor(tgui::Color::Red);
        alleEingabenValid = false;
    }
    validiereSimulationsParameter();
}





// Validiert die Standardabweichung und überprüft die Beziehung zum Mittelwert
void SimulationsGUI::validiereStandardabweichung(tgui::EditBox::Ptr SD_edit, tgui::EditBox::Ptr mean_Edit) {
    try {
        double wert = std::stod(SD_edit->getText().toStdString());
        double mittelwertTemp = std::stod(mean_Edit->getText().toStdString());

        // Prüfe nur die Grundbedingungen für die Standardabweichung
        if (wert >= 1 && wert < mittelwertTemp) {
            standardabweichung = wert;
            SD_edit->getRenderer()->setBackgroundColor(tgui::Color::White);
        }
        else {
            standardabweichung = 0;
            SD_edit->getRenderer()->setBackgroundColor(tgui::Color::Red);
            alleEingabenValid = false;
        }

        // Die Gesamtvalidierung wird separat durchgeführt
        validiereSimulationsParameter();
    }
    catch (...) {
        standardabweichung = 0;
        SD_edit->getRenderer()->setBackgroundColor(tgui::Color::Red);
        alleEingabenValid = false;
    }
}






// Startet die Simulation in einem separaten Thread, wenn Eingaben gültig sind
void SimulationsGUI::starteSimulationImHintergrund() {
    if (!alleEingabenValid) {
        std::cerr << "Fehler: Ungueltige Eingaben!" << std::endl;
        return;
    }


    // Führt die Simulation asynchron aus, sodass das GUI nicht blockiert wird
    auto simulationFuture = std::async(std::launch::async, [this]() {		//[3]
        starteSimulation();
        });

    window.close();
}




// Initialisiert alle GUI-Elemente
void SimulationsGUI::initialisiereBenutzerInterface() {
    auto titel = tgui::Label::create("Simulationsparameter");
    titel->setPosition(10, 10);
    titel->setTextSize(20);
    gui.add(titel);

    // Mittelwert-Eingabefeld und Label
    auto mittelwertLabel = tgui::Label::create("Mittelwert der Auftraege pro Tag [1-3000]:");
    mittelwertLabel->setPosition(10, 50);
    gui.add(mittelwertLabel);

    auto mean_Edit = tgui::EditBox::create();
    mean_Edit->setPosition(10, 80);
    mean_Edit->setSize(200, 30);
    mean_Edit->setInputValidator(tgui::EditBox::Validator::Float);
    mean_Edit->onTextChange([this, mean_Edit]() {				//[12]
        validiereMittelwert(mean_Edit);
        });
    gui.add(mean_Edit);

    // Standardabweichungs-Eingabefeld und Label
    auto SD_label = tgui::Label::create("Standardabweichung der Auftraege: [1-Mittelwert]");
    SD_label->setPosition(10, 120);						//[7]
    gui.add(SD_label);

    auto SD_edit = tgui::EditBox::create();
    SD_edit->setPosition(10, 150);
    SD_edit->setSize(200, 30);
    SD_edit->setInputValidator(tgui::EditBox::Validator::Float);
    SD_edit->onTextChange([this, SD_edit, mean_Edit]() {
        validiereStandardabweichung(SD_edit, mean_Edit);
        });
    gui.add(SD_edit);

    // Auswahl für saisonale Einflüsse
    auto saisonLabel = tgui::Label::create("Saisonale Einfluesse beruecksichtigen?");
    saisonLabel->setPosition(10, 190);
    gui.add(saisonLabel);

    auto jaButton = tgui::Button::create("Ja");
    jaButton->setPosition(10, 220);
    jaButton->setSize(90, 30);
    jaButton->onClick([this]() {
        saisonaleEinfluesse = true;
        starteSimulationImHintergrund();
        });
    gui.add(jaButton);

    auto neinButton = tgui::Button::create("Nein");
    neinButton->setPosition(110, 220);
    neinButton->setSize(90, 30);							//[6]
    neinButton->onClick([this]() {							//[8]
        saisonaleEinfluesse = false;
        starteSimulationImHintergrund();
        });
    gui.add(neinButton);
}





// Überprüft die Gesamtvalidität der Simulationsparameter
void SimulationsGUI::validiereSimulationsParameter() {
    bool mittelwertValid = (mittelwert >= 1 && mittelwert <= 3000);
    bool standardabweichungValid = (standardabweichung >= 1 && standardabweichung < mittelwert);

    alleEingabenValid = mittelwertValid && standardabweichungValid;
}




// Führt die Simulation durch, einschließlich der Ergebnisaufzeichnung
void SimulationsGUI::starteSimulation() {


    const float baseAmplitude = mittelwert * SAISON_BASIS_AMPLITUDE;

    int gesamtMitarbeiterbedarf = 0;
    int verbleibendeAuftraegeVortag = 0;

    std::vector<double> tage;
    std::vector<double> mitarbeiter;
    tage.reserve(365);
    mitarbeiter.reserve(365);

    for (int tag = 1; tag <= 365; ++tag) {
        std::cout << ("\nTag " + std::to_string(tag) + ":") << std::endl;   

        double adjustedMean = mittelwert;                                                       // Mean = Mittelwert, kürzer auf Englisch; AdjustMean = Angepasster Mittelwert
        if (saisonaleEinfluesse) {
            float sommerEffekt = std::sin(frequenz * (tag - 45));			//[3]
            float weihnachtsEffekt = 0.5f * std::sin(frequenz * (tag + 100));
            adjustedMean = mittelwert + baseAmplitude * (sommerEffekt + weihnachtsEffekt);
            adjustedMean = std::max(mittelwert * 0.2, adjustedMean);		//[3]
        }

        // Simulation eines einzelnen Tages und Berechnung des Mitarbeiterbedarfs
        Simulation simulation(adjustedMean, standardabweichung,
            meanProdukteProAuftrag, stdProdukteProAuftrag,
            verbleibendeAuftraegeVortag);
        simulation.simuliereTag();

        verbleibendeAuftraegeVortag = simulation.getVerbleibendeAuftraege();
        gesamtMitarbeiterbedarf += simulation.getTagesMitarbeiterBedarf();

        tage.push_back(static_cast<double>(tag));
        mitarbeiter.push_back(static_cast<double>(simulation.getTagesMitarbeiterBedarf()));

        std:: cout << ("Verbleibende Auftraege fuer Folgetag: " + std::to_string(verbleibendeAuftraegeVortag)) << std::endl;  


    }




    // Erzeugt ein Diagramm der Ergebnisse und speichert es als PNG
    bool success;
    StringReference* errorMessage = CreateStringReferenceLengthValue(0, L' ');	                    //[10]
    RGBABitmapImageReference* imageReference = CreateRGBABitmapImageReference();							                        //[10]

    std::vector<double>* tagePtr = new std::vector<double>(tage);
    std::vector<double>* mitarbeiterPtr = new std::vector<double>(mitarbeiter);

    //Funktion zur Erstellung des Graphens
    success = DrawScatterPlot(imageReference, 1200, 800, tagePtr, mitarbeiterPtr, errorMessage);    										                    //[10]





    //Ausgabe vom Mittelwert und von der Standardabweicung sollen nur 2 Nachkommastellen haben
    std::stringstream stream_mean;
    stream_mean << std::fixed << std::setprecision(2) << mittelwert;			                                                //[3]
    std::stringstream stream_SD;
    stream_SD << std::fixed << std::setprecision(2) << standardabweichung;		                                                //[3]




    if (success) {
        std::vector<double>* pngdata = ConvertToPNG(imageReference->image);	                                                //[10]
        WriteToFile(pngdata, "mitarbeiterbedarf.png");					                                                //[10]
        DeleteImage(imageReference->image);						                                                //[10]

        logLine("\nGraph wurde als 'mitarbeiterbedarf.png' gespeichert.");
        logLine("Protokoll wurde als 'Protokoll.txt' gespeichert.");
        logLine("\n\n\n\n\n\nEs wurde mit einem Mittelwert fuer die Auftraege von: "
            + stream_mean.str() + " Simuliert. Zu diesem Mittelwert wurde eine Standartabweichung von: " 
            + stream_SD.str() + " gewaehlt");

        if (saisonaleEinfluesse) {
            logLine("\nDie Simulation erfolgte mit Seasionalen Einfluessen");
        }
        else {
            logLine("\nDie Simulation erfolgte ohne Seasionale Einfluessen");
        }

        system("start mitarbeiterbedarf.png");  //Mitarbeiterbedarf.png / Graph wird direkt geöffnet											//[4]
        //system("xdg-open mitarbeiterbedarf.png");  //Für Linux-Systeme
    }
    else {
        logLine("Fehler beim Erstellen des Graphen.");
    }

    logLine("\nDurchschnittlicher Mitarbeiterbedarf: " +
        std::to_string(static_cast<double>(gesamtMitarbeiterbedarf) / 365) + "\n\n\n\n\n");



    std::ofstream protokoll("Protokoll.txt");						                                             //[1]
    protokoll << logStream.str();
    protokoll.close();


    delete tagePtr;
    delete mitarbeiterPtr;
    FreeAllocations();									                                            //[10]
}




//GUI-Schleife
void SimulationsGUI::starteSimulationsGUI() {
    while (window.isOpen()) {
        sf::Event event;									                                     //[7]
        while (window.pollEvent(event)) {
            gui.handleEvent(event);

            if (event.type == sf::Event::Closed) {					
                window.close();
            }
        }

        window.clear(sf::Color(240, 240, 240));					                                                    //[9]
        gui.draw();
        window.display();								                                            //[9]
    }
}
