# Loctek-Tisch

Ein Loctek-Motion- oder Flexispot-Tisch auf dem Bildschirm. Die Kachel zeigt die Höhe des Tischs mit einem Balken für
seine Position. Ihre Tasten schicken den Tisch in Sitz- oder Stehhöhe, fahren ihn hoch oder runter, halten ihn an und
rufen die Speicherplätze auf. Du wählst, welche Tasten die Kachel hat, und sie zeichnet nur diese.

> Die Kachel arbeitet mit den Entitäten des Pakets [LoctekMotion_IoT](https://github.com/iMicknl/LoctekMotion_IoT):
> ein ESP32 an der Steuerung des Tischs, eingerichtet in Home Assistant. Richte das zuerst ein.

## Einrichten

1. Öffne in Tessera **Plugins**, wähle **Mit Link hinzufügen** und füge `https://github.com/Orishas/tessera-loctek-desk`
   ein.
2. Füge das Plugin einem Screen hinzu. Der Screen baut einmal neu.
3. Setze im **Layout** die Kachel **Tisch** aus der Gruppe Plugins der Bibliothek.
4. Wähle im Inspektor die Abdeckung (cover) des Tischs, die Entität **Desk**. Tipp: Tippe den Namen des Tischs in die
   Liste, um sie zu finden.
5. Wähle, was du hast. Alles ist optional, und die Kachel lässt eine Taste weg, deren Entität du nicht gewählt hast:

   | Option | Entität des LoctekMotion-Pakets |
   |---|---|
   | **Höhe** | Der Sensor **Desk height**. Ohne ihn zeigt die Kachel die Position der Abdeckung in Prozent. |
   | **Sitzen** | Die Taste **Sit** (bei manchen Bedienteilen Preset 4). |
   | **Stehen** | Die Taste **Stand** (bei manchen Bedienteilen Preset 3). |
   | **Speicher 1** | Die Taste **Preset 1**. |
   | **Speicher 2** | Die Taste **Preset 2**. |
   | **Kindersicherung** | Der Schalter **Child lock**. Die Kachel zeigt ihn als Taste **Sperre**. |
   | **Alarm** | Die Taste **Alarm**, oder der Schalter **Alarm off**, wenn dein Paket den hat. |
   | **Handtasten** | Standardmäßig an. Die Tasten **Hoch**, **Stopp** und **Runter**. |

## Größen

| Größe | Zeigt |
|---|---|
| 1 x 1 | Nur die Höhe. Ein Tippen schickt den Tisch ans andere Ende: nach Stehen, solange er unter 50 % steht, sonst nach Sitzen. Fährt der Tisch, hält ein Tippen ihn an. Ein Tippen tut nichts, wenn die nötige Taste keine Entität hat oder die Abdeckung keine Position meldet. |
| 2 x 1 | Links die Höhe, rechts eine Reihe Tasten. |
| 1 x 2 | Oben die Höhe, darunter zwei Reihen Tasten. |
| 2 x 2 | Oben die Höhe, darunter drei Reihen Tasten. Mit allen vier Gruppen teilen sich die Speichertasten die dritte Reihe mit Sperre und Alarm. |

Die Tasten kommen in vier Gruppen, in dieser Reihenfolge: Sitzen und Stehen; Hoch, Stopp und Runter; 1 und 2; Sperre
und Alarm. Eine Gruppe ohne Taste gibt es nicht. Jede Größe füllt ihre Reihen mit den Gruppen in dieser Reihenfolge,
und Gruppen, die keine Reihe finden, werden nicht gezeichnet.

Eine Kachel, die breiter als hoch ist, etwa 2 x 2 oder 1 x 2 auf einem kleinen Screen wie dem CYD, zeigt links die
Höhe und rechts eine Reihe Tasten, wenn unter der Höhe keine Reihe passt. Wo gar keine Tasten passen, zeigt die Kachel
nur die Höhe, und ein Tippen schickt den Tisch ans andere Ende, wie bei 1 x 1.

Eine Taste wird nie gequetscht: Eine Reihe, die auf einen kleinen Screen nicht passt, entfällt ganz.

Fährt der Tisch, werden die Höhe und die Taste **Stopp** in der Akzentfarbe gezeichnet.

## Gut zu wissen

- **Hoch** und **Runter** starten eine Fahrt, die läuft, bis du **Stopp** drückst, die andere Richtung wählst oder der
  Fahrbereich zu Ende ist. Den Bereich setzt das Paket durch. Eine Kachel eines Plugins kennt nur das Tippen, kein
  Halten; eine Taste kann den Tisch also nicht nur fahren lassen, solange du sie hältst.
- Die Taste **Sperre** zeigt nicht, ob der Tisch gesperrt ist: Die Steuerung meldet das nicht. Sie drückt die
  Kindersicherung fünf Sekunden lang, und solange der Schalter hält, ist die Taste auf einem bernsteinfarbenen Grund
  gezeichnet.
- Eine Taste ist nur gedämpft und nimmt kein Tippen an, solange ihre Entität `unavailable` ist. Eine Taste, die noch
  nie gedrückt wurde (Zustand `unknown`), funktioniert.
- Kann Home Assistant eine Aktion nicht ausführen, zeigt die Taste zwei Sekunden lang die Warnfarbe.
- Die Höhe erscheint erst, wenn die Steuerung des Tischs aufgewacht ist. Das Paket weckt sie zehn Sekunden nach dem
  Start seines ESP. Bis dahin zeigt die Kachel die Position in Prozent. Eine Höhe in Zoll bleibt in Zoll; die Kachel
  rechnet nichts um.
- Der Screen muss Aktionen in Home Assistant ausführen dürfen (Tessera schaltet das ein, wenn es den Screen anlegt).
- Auf einem Guition 4", der mit **Rotation** gedreht ist, kann mit der Firmware 0.60.0 ein Tippen auf die Kachel eines
  Plugins verloren gehen. Nimm Rotation 0°.
- Dieses Plugin steht in keiner Verbindung zu Loctek, Flexispot oder dem Projekt LoctekMotion_IoT.
