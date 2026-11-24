# Changelog

## 0.2.1 - 2026-11-24
- Das README sagt, wie die Seite des Tischs eingerichtet wird: welche Tische passen, die Teile, die Verdrahtung, das
  Paket zum Flashen und die Entitäten, die es liefert. Es sagt auch, auf welchen Screens die Kachel ausprobiert wurde,
  getrennt nach echtem Screen und Vorschau-Werkzeug.
- Behoben: Der Hinweis der Option **Alarm** sagte, die Taste löse den Alarm des Tischs aus. Sie drückt die Alarmtaste
  des Bedienteils, und mit dem Schalter **Alarm off** hält sie sie drei Sekunden lang.
- Das `memory` der Kachel zählt die Masken von Sitzen und Stehen so, wie ein Board ohne PSRAM sie hält: 7900 Bytes,
  vorher 4300.

## 0.2.0 - 2026-11-17
- Neu: **Aus einem Gerät füllen** über den Optionen. Wähle dort den ESP des Tisches, und die Kachel nimmt seinen
  Höhensensor und seine Tasten Sitzen, Stehen, Preset 1, Preset 2, Kindersicherung und Alarm auf einmal. Die Liste
  jeder Option zeigt außerdem zuerst die passende Entität, mit einer Taste für alle Entitäten.
- Braucht Tessera 0.4.98 oder neuer (Plugin-API 0.26).

## 0.1.0 - 2026-11-11
- Erste Version: eine Kachel, die die Höhe eines Loctek-Motion- oder Flexispot-Tischs zeigt und den Tisch fährt, mit
  den Entitäten des LoctekMotion-Pakets.
- Acht Optionen: die Höhe, die Tasten Sitzen, Stehen, Speicher 1, Speicher 2, Kindersicherung und Alarm sowie die
  Handtasten Hoch, Stopp und Runter. Sitzen und Stehen tragen einen Sitz und einen stehenden Menschen, die anderen
  Tasten ihre Symbole oder Zahlen.
- Vier Größen, 1 x 1 bis 2 x 2. Bei 1 x 1 schickt ein Tippen den Tisch ans andere Ende.
