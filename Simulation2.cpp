#include "simulation2.hpp"
#include <algorithm>									                                                                           //[3]
#include <iostream>




// Konstruktor für die Simulation-Klasse: Initialisiert die Zufallsverteilungen und Tagesaufträge                                  [Julian]
Simulation::Simulation(double mean_AuftraegeProTag, double SD_AuftraegeProTag,
    double meanProdukteProAuftrag, double SD_ProdukteProAuftrag,
    int verbleibendeAuftraege)
    : auftraegeDistribution(mean_AuftraegeProTag, SD_AuftraegeProTag)
    , produkteDistribution(meanProdukteProAuftrag, SD_ProdukteProAuftrag)
    , basiszeitDist(BASISZEIT_MEAN, BASISZEIT_SD)
    , wegzeitDist(WEGZEIT_MEAN, WEGZEIT_SD)
    , greifzeitDist(GREIFZEIT_MEAN, GREIFZEIT_SD)
    , totzeitDist(TOTZEIT_MEAN, TOTZEIT_SD)



{
    auto& rng = RandomGenerator::holeZufallsgenerator();					                                                       //[3]

    tagesAuftraege = std::max(0, static_cast<int>(std::round(rng.erzeugeZufallswert(auftraegeDistribution))));                     //[3], [13]
    // Berechne tagesAuftraege basierend auf einer normalverteilten Zufallszahl						       

    tagesAuftraege += verbleibendeAuftraege;    // Füge verbleibende Aufträge hinzu
}


// Generiert die Tagesaufträge und deren Artikel mit Zeitkomponenten                                                                [Julian] [Deniz]
void Simulation::generiereAuftraege() {
    auto& rng = RandomGenerator::holeZufallsgenerator();
    auftragsListe.clear();
    auftragsListe.reserve(tagesAuftraege);  // Speicher für Aufträge reservieren

    // Bestellzeitfenster festlegen
    double zeitStart = 4.0;
    double zeitEnde = 21.0;

    for (int i = 0; i < tagesAuftraege; ++i) {
        Auftrag auftrag;



        //Verteilung der Bestellzeiten
        auftrag.bestellZeit = zeitStart + (zeitEnde - zeitStart) * (static_cast<double>(i) / tagesAuftraege);


        // Artikel pro Auftrag generieren
        int artikelAnzahl = static_cast<int>(std::max(1.0, std::round(rng.erzeugeZufallswert(produkteDistribution))));
        auftrag.artikel.reserve(artikelAnzahl); // Speicher für Artikel reservieren													 //[13]

        for (int j = 0; j < artikelAnzahl; ++j) {
            Artikel artikel;
            // Zeitkomponenten für jeden Artikel generieren
            artikel.zeiten = generiereZeitkomponenten();
            auftrag.artikel.push_back(std::move(artikel));
        }
        auftragsListe.push_back(std::move(auftrag));																		         //[3]
    }

    // Sortiere die Aufträge nach Bestellzeit
    std::sort(auftragsListe.begin(), auftragsListe.end(),
        [](const Auftrag& a, const Auftrag& b) { return a.bestellZeit < b.bestellZeit; });											 //[3]
}

// Generiert die Zeitkomponenten (Basis-, Weg-, Greif- und Totzeit) für einen Artikel                                                //[Can]
Zeitkomponenten Simulation::generiereZeitkomponenten() {
    auto& rng = RandomGenerator::holeZufallsgenerator();
    Zeitkomponenten zeiten;
    zeiten.basiszeit = rng.erzeugeZufallswert(basiszeitDist);
    zeiten.wegzeit = rng.erzeugeZufallswert(wegzeitDist);
    zeiten.greifzeit = rng.erzeugeZufallswert(greifzeitDist);
    zeiten.totzeit = rng.erzeugeZufallswert(totzeitDist);
    return zeiten;
}


// Überprüft, ob der angegebene Zeitpunkt innerhalb einer Pausenzeit liegt                                                           [Can]
bool Simulation::istPausenzeit(double zeitpunkt) {
    for (const auto& pause : PAUSENZEITEN) {
        if (zeitpunkt >= pause.first && zeitpunkt < pause.second) {
            return true;
        }
    }
    return false;
}

// Bearbeitet die Aufträge und berechnet die Gesamtbearbeitungszeit des Tages                                                        [Julian] [Deniz]
int Simulation::bearbeiteAuftraege() {
    int totalBearbeitungszeit = 0;
    std::vector<Auftrag> verbleibend;
    verbleibend.reserve(auftragsListe.size());

    // Simulation von Arbeitsstunden von 4:00 bis 20:00 Uhr in 30-Minuten-Intervallen
    for (double stunde = 4.0; stunde <= 20.0; stunde += 0.5) {
        if (istPausenzeit(stunde)) continue;    // Pausenzeit überspringen

        for (const auto& auftrag : auftragsListe) {
            if (auftrag.bestellZeit <= stunde) {
                // Bearbeite alle Artikel im Auftrag
                for (const auto& artikel : auftrag.artikel) {
                    totalBearbeitungszeit += artikel.zeiten.gesamtzeit();
                }
            }
            else {
                // Auftrag für später aufheben
                verbleibend.push_back(auftrag);
            }
        }
        // Update auftragsListe mit verbleibenden Aufträgen
        auftragsListe = std::move(verbleibend);
        verbleibend.clear();																					                      //[3]
        verbleibend.reserve(auftragsListe.size());
    }

    return totalBearbeitungszeit;
}



// Berechnet die benötigte Mitarbeiteranzahl basierend auf der Bearbeitungszeit                                                       [Julian] [Deniz]
int Simulation::berechneMitarbeiter(int totalBearbeitungszeit) {
    return (totalBearbeitungszeit + ARBEITSSEKUNDEN_PRO_MITARBEITER - 1) / ARBEITSSEKUNDEN_PRO_MITARBEITER;
}



// Führt eine tägliche Simulation durch: generiert Aufträge und bearbeitet sie
void Simulation::simuliereTag() {
    generiereAuftraege();
    int totalBearbeitungszeit = bearbeiteAuftraege();
    tagesMitarbeiterBedarf = berechneMitarbeiter(totalBearbeitungszeit);


    std::cout << ("Auftraege heute: " + std::to_string(tagesAuftraege)) << std::endl;
    std::cout << ("Mitarbeiter: " + std::to_string(tagesMitarbeiterBedarf)) << std::endl;

}
