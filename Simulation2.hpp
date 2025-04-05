#pragma once								                                                                        //[13]
#include <vector> 								                                                                    //[13]
#include <random>								                                                                    //[13]
#include <memory>								                                                                    //[13]
#include <array>								                                                                    //[13]
#include <chrono>								                                                                    //[13]
#include <sstream>								                                                                    //[13]

// Struktur zur Speicherung der Zeitkomponenten für die Bearbeitung eines Artikels
struct Zeitkomponenten {
    int basiszeit;
    int wegzeit;
    int greifzeit;
    int totzeit;
    // Berechnet die Gesamtzeit als Summe aller Zeitkomponenten
 int gesamtzeit() const {
        return basiszeit + wegzeit + greifzeit + totzeit;
    }
};



struct Artikel {
    Zeitkomponenten zeiten;          // Zeitkomponenten für jeden Artikel
};

struct Auftrag {
    double bestellZeit;             // Zeitpunkt der Bestellung
    std::vector<Artikel> artikel;   // Liste der Artikel in diesem Auftrag
};

// Singleton-Pattern zur Generierung von Zufallszahlen
class RandomGenerator {
public:
    static RandomGenerator& holeZufallsgenerator() {
        // Gibt die einzige Instanz des Zufallsgenerators zurück
        static RandomGenerator instance;
        return instance;
    }

    // Generiert einen Zufallswert basierend auf einer Normalverteilung
    template<typename T>																					        //[13]
    T erzeugeZufallswert(std::normal_distribution<T> dist) {
        return std::max(T(1), static_cast<T>(std::round(dist(generator))));
    }

private:
    // Konstruktor initialisiert den Zufallsgenerator mit einem Zeitstempel
    RandomGenerator() {
        generator.seed(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));     //[13]
    }
    std::default_random_engine generator;																		    //[13]
};


// Klasse zur Simulation des täglichen Auftragsaufkommens und der Bearbeitung
class Simulation {
public:
    // Konstruktor: Initialisiert Simulation mit Mittelwert und Standardabweichung für Aufträge und Produkte pro Auftrag
    Simulation(double mean_AuftraegeProTag, double SD_AuftraegeProTag,
        double meanProdukteProAuftrag, double SD_ProdukteProAuftrag,
        int verbleibendeAuftraege);

    // Simuliert einen Arbeitstag und berechnet die Mitarbeiteranzahl
    void simuliereTag();

    // Gibt die Anzahl der verbleibenden Aufträge am Tagesende zurück
    int getVerbleibendeAuftraege() const { return auftragsListe.size(); }								//[6]
    // Gibt die berechnete Mitarbeiteranzahl für den Tag zurück
    int getTagesMitarbeiterBedarf() const { return tagesMitarbeiterBedarf; }

private:
    // Pausenzeiten im Format {Startzeit, Endzeit} (in Stunden)
    static constexpr std::array<std::pair<double, double>, 5> PAUSENZEITEN = { {									//[13]
        {6.5, 7.0}, {09.5, 10.0}, {12.00, 13.00}, {15.0, 15.5}, {17.5, 18.0}
    } };
    // Arbeitssekunden pro Mitarbeiter pro Tag (6,5 Stunden)
    static constexpr int ARBEITSSEKUNDEN_PRO_MITARBEITER = 23400; // 6.5 * 3600										//[13]

    int tagesAuftraege;                     // Anzahl der Tagesaufträge
    int tagesMitarbeiterBedarf;             // Benötigte Mitarbeiteranzahl für den Tag
    std::vector<Auftrag> auftragsListe;     // Liste der Aufträge für den Tag

    std::normal_distribution<double> auftraegeDistribution; // Verteilung für Tagesaufträge							//[13]
    std::normal_distribution<double> produkteDistribution;  // Verteilung für Produkte pro Auftrag
    std::normal_distribution<double> basiszeitDist;         // Verteilung für Basiszeit
    std::normal_distribution<double> wegzeitDist;           // Verteilung für Wegzeit
    std::normal_distribution<double> greifzeitDist;         // Verteilung für Greifzeit
    std::normal_distribution<double> totzeitDist;           // Verteilung für Totzeit


    // Generiert die Tagesaufträge mit Artikeln und Bestellzeiten
    void generiereAuftraege();


    // Bearbeitet alle Aufträge und berechnet die Gesamtbearbeitungszeit
    int bearbeiteAuftraege();

    // Berechnet die benötigte Mitarbeiteranzahl basierend auf der Gesamtbearbeitungszeit
    static int berechneMitarbeiter(int totalBearbeitungszeit);

    // Überprüft, ob ein Zeitpunkt in den Pausenzeiten liegt
    static bool istPausenzeit(double zeitpunkt);

    // Generiert die Zeitkomponenten für einen Artikel
    Zeitkomponenten generiereZeitkomponenten();


    // Konstanten für Zeitverteilungen
    static constexpr double BASISZEIT_MEAN = 60.0;
    static constexpr double BASISZEIT_SD = 5.0;
    static constexpr double WEGZEIT_MEAN = 300.0;
    static constexpr double WEGZEIT_SD = 20.0;
    static constexpr double GREIFZEIT_MEAN = 45.0;
    static constexpr double GREIFZEIT_SD = 3.0;
    static constexpr double TOTZEIT_MEAN = 20.0;
    static constexpr double TOTZEIT_SD = 2.0;

};
