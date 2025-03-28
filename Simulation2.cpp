#include "simulation2.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

// Constructor with more descriptive initialization and error checking
Simulation::Simulation(
    double mean_AuftraegeProTag,
    double SD_AuftraegeProTag,
    double meanProdukteProAuftrag,
    double SD_ProdukteProAuftrag,
    int verbleibendeAuftraege
) : auftraegeDistribution(mean_AuftraegeProTag, SD_AuftraegeProTag),
produkteDistribution(meanProdukteProAuftrag, SD_ProdukteProAuftrag),
basiszeitDist(BASISZEIT_MEAN, BASISZEIT_SD),
wegzeitDist(WEGZEIT_MEAN, WEGZEIT_SD),
greifzeitDist(GREIFZEIT_MEAN, GREIFZEIT_SD),
totzeitDist(TOTZEIT_MEAN, TOTZEIT_SD)
{
    // Robust random generator retrieval with potential fallback
    RandomGenerator& randomGen = RandomGenerator::holeZufallsgenerator();

    // More nuanced rounding and clamping of daily orders
    int randomOrders = std::max(0,
        static_cast<int>(std::round(randomGen.erzeugeZufallswert(auftraegeDistribution)))
    );

    tagesAuftraege = randomOrders + verbleibendeAuftraege;
}

// Enhanced order generation with more explicit time window calculation
void Simulation::generiereAuftraege() {
    auto& rng = RandomGenerator::holeZufallsgenerator();

    // Clear previous orders and pre-allocate memory
    auftragsListe.clear();
    auftragsListe.reserve(tagesAuftraege);

    const double zeitStart = 4.0;
    const double zeitEnde = 21.0;
    const double zeitIntervall = zeitEnde - zeitStart;

    for (int orderIndex = 0; orderIndex < tagesAuftraege; ++orderIndex) {
        Auftrag currentOrder;

        // More precise time distribution calculation
        currentOrder.bestellZeit = zeitStart +
            (zeitIntervall * (static_cast<double>(orderIndex) / std::max(1, tagesAuftraege - 1)));

        // Robust article count generation
        int artikelAnzahl = std::max(1,
            static_cast<int>(std::round(rng.erzeugeZufallswert(produkteDistribution)))
        );

        currentOrder.artikel.reserve(artikelAnzahl);

        for (int articleIndex = 0; articleIndex < artikelAnzahl; ++articleIndex) {
            Artikel article;
            article.zeiten = generiereZeitkomponenten();
            currentOrder.artikel.push_back(std::move(article));
        }

        auftragsListe.push_back(std::move(currentOrder));
    }

    // Lambda comparison with explicit const references
    std::sort(auftragsListe.begin(), auftragsListe.end(),
        [](const Auftrag& orderA, const Auftrag& orderB) {
            return orderA.bestellZeit < orderB.bestellZeit;
        });
}

// Time component generation remains largely the same, with minor readability improvements
Zeitkomponenten Simulation::generiereZeitkomponenten() {
    auto& rng = RandomGenerator::holeZufallsgenerator();
    Zeitkomponenten timeComponents;

    timeComponents.basiszeit = rng.erzeugeZufallswert(basiszeitDist);
    timeComponents.wegzeit = rng.erzeugeZufallswert(wegzeitDist);
    timeComponents.greifzeit = rng.erzeugeZufallswert(greifzeitDist);
    timeComponents.totzeit = rng.erzeugeZufallswert(totzeitDist);

    return timeComponents;
}

// Break time check with more robust handling
bool Simulation::istPausenzeit(double currentTime) {
    return std::any_of(PAUSENZEITEN.begin(), PAUSENZEITEN.end(),
        [currentTime](const auto& pausePeriod) {
            return currentTime >= pausePeriod.first && currentTime < pausePeriod.second;
        });
}

// Order processing with more explicit time management
int Simulation::bearbeiteAuftraege() {
    int totalProcessingTime = 0;
    std::vector<Auftrag> remainingOrders;
    remainingOrders.reserve(auftragsListe.size());

    // Explicit work hours with half-hour increments
    for (double workHour = 4.0; workHour <= 20.0; workHour += 0.5) {
        if (istPausenzeit(workHour)) continue;

        // Separate processing and remaining order tracking
        for (const auto& order : auftragsListe) {
            if (order.bestellZeit <= workHour) {
                for (const auto& article : order.artikel) {
                    totalProcessingTime += article.zeiten.gesamtzeit();
                }
            }
            else {
                remainingOrders.push_back(order);
            }
        }

        // Efficient order list update
        auftragsListe = std::move(remainingOrders);
        remainingOrders.clear();
        remainingOrders.reserve(auftragsListe.size());
    }

    return totalProcessingTime;
}

// Staff calculation with more explicit rounding strategy
int Simulation::berechneMitarbeiter(int totalBearbeitungszeit) {
    return std::ceil(static_cast<double>(totalBearbeitungszeit) / ARBEITSSEKUNDEN_PRO_MITARBEITER);
}

// Simulation day method with improved output formatting
void Simulation::simuliereTag() {
    generiereAuftraege();
    int totalProcessingTime = bearbeiteAuftraege();
    tagesMitarbeiterBedarf = berechneMitarbeiter(totalProcessingTime);

    // More robust output with better formatting
    std::cout << "Tagesaufträge: " << tagesAuftraege << std::endl;
    std::cout << "Benötigte Mitarbeiter: " << tagesMitarbeiterBedarf << std::endl;
}
