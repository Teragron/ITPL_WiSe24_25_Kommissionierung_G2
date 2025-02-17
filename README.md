# Tägliche Simulation von Aufträgen und Personalbedarf

Dieses Projekt simuliert die Bearbeitung von Aufträgen und berechnet den Personalbedarf für einen täglichen Betrieb. Die Simulation berücksichtigt unterschiedliche Verteilungsmodelle für Aufträge und Produkte sowie benutzerdefinierte Parameter über eine grafische Benutzeroberfläche (GUI).

## Funktionen

- **Benutzeroberfläche:** Eine grafische Benutzeroberfläche zur Eingabe von Simulationsparametern, einschließlich Mittelwert und Standardabweichung für Aufträge pro Tag und Produkte pro Auftrag.
- **Simulation:** Generierung von Aufträgen und Berechnung der Bearbeitungszeit für jeden Auftrag, unter Berücksichtigung verschiedener Zeitkomponenten wie Basiszeit, Laufzeit, Greifzeit und Ausfallzeit.
- **Personalbedarf:** Berechnung des erforderlichen Personals basierend auf der gesamten Bearbeitungszeit.
- **Saisonale Anpassungen:** Möglichkeit, saisonale Einflüsse zu berücksichtigen, die die Anzahl der Aufträge pro Tag beeinflussen.
- **Visualisierung:** Grafische Ausgabe, die den Personalbedarf für jeden Tag über das Jahr hinweg anzeigt.

## Voraussetzungen

- SFML (Simple and Fast Multimedia Library)
- tgui (TGUI - eine GUI-Bibliothek)

## Installation

### 1. Repository klonen

```bash
git clone https://github.com/teragron/programm_test.git
cd programm_test
