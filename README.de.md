# Loctek-Tisch

Ein Loctek-Motion- oder Flexispot-Tisch auf dem Bildschirm. Die Kachel zeigt die Höhe des Tischs mit einem Balken für
seine Position. Ihre Tasten schicken den Tisch in Sitz- oder Stehhöhe, fahren ihn hoch oder runter, halten ihn an und
rufen die Speicherplätze auf. Du wählst, welche Tasten die Kachel hat, und sie zeichnet nur diese.

Die Kachel arbeitet mit den Entitäten des Pakets [LoctekMotion_IoT](https://github.com/iMicknl/LoctekMotion_IoT): ein
ESP32 an der Steuerbox des Tischs, mit ESPHome in Home Assistant eingerichtet.

## Einrichten

### Der Tisch

Mach das zuerst. Es ist die Arbeit des Projekts LoctekMotion_IoT, und dessen README hilft weiter, wenn bei deinem
Tisch etwas anders ist.

1. **Prüfe, ob dein Tisch passt.** Er braucht eine Steuerbox von Loctek Motion mit einer RJ45-Buchse für das
   Bedienteil: Tische von Flexispot haben eine, ebenso andere Marken mit Teilen von Loctek Motion. Das Projekt führt
   die [Bedienteile und Tische, die nachweislich gehen](https://github.com/iMicknl/LoctekMotion_IoT#supported-control-panels) (darunter E7 Pro, E7Q, E2Q, E5Q
   und EC5), und sagt, dass eine Verdrahtung für jedes Bedienteil mit so einer Buchse passen sollte.
2. **Besorge die Teile.** Ein ESP32-Board (das Paket ist für ein ESP32 NodeMCU geschrieben), ein Netzwerkkabel und
   eine RJ45-Adapterplatine mit Steckbrücken, wenn du das Kabel nicht aufschneiden und löten willst.
3. **Verdrahte es**, wie die [Pinbelegung](https://github.com/iMicknl/LoctekMotion_IoT#pin-out) des Projekts zeigt: Fünf Adern des Kabels gehen an den ESP32,
   +5V an VIN, GND an GND, TX an GPIO17, RX an GPIO16 und PIN 20 an GPIO23. Der Tisch versorgt den ESP32 mit Strom.
   Hat die Steuerbox zwei Buchsen für ein Bedienteil (beschriftet mit HS), kommt der ESP32 in die freie, und das
   Bedienteil bleibt in der anderen. Nimm nie eine Motorbuchse (beschriftet mit M). Eine Steuerbox mit nur einer
   Buchse braucht den [Durchschleif-Aufbau](https://github.com/iMicknl/LoctekMotion_IoT#pass-through-configuations) des Projekts und dessen anderes Paket.
4. **Flashe den ESP32** mit ESPHome und
   [`packages/office-desk-esp32.yaml`](https://github.com/iMicknl/LoctekMotion_IoT/blob/main/packages/office-desk-esp32.yaml) des Projekts. Trage dort dein
   WLAN ein, und die Pins, falls du andere verdrahtet hast.
5. **Nimm ihn in Home Assistant auf.** Home Assistant findet den ESP32 von selbst: Füge ihn unter
   **Einstellungen > Geräte & Dienste** mit der Integration **ESPHome** hinzu.

Das Gerät hat dann diese Entitäten, und genau die nehmen die Optionen der Kachel:

| Entität | Art | In der Kachel |
|---|---|---|
| **Desk** | Abdeckung (cover) | Die Entität der Kachel: die Position und die Tasten Hoch, Stopp und Runter. |
| **Desk height** | Sensor | Die Option **Höhe**. |
| **Sit**, **Stand** | Taste (button) | Die Optionen **Sitzen** und **Stehen**. |
| **Preset 1**, **Preset 2** | Taste (button) | Die Optionen **Speicher 1** und **Speicher 2**. |
| **Child lock** | Schalter (switch) | Die Option **Kindersicherung**. |
| **Alarm off** | Schalter (switch) | Die Option **Alarm**. Das Paket hat auch eine Taste **Alarm**. |

Fahre den Tisch einmal aus Home Assistant, bevor du weitermachst: Bewegt die Abdeckung **Desk** ihn und zeigt
**Desk height** eine Höhe, ist die Seite des Tischs fertig.

### Die Kachel

1. Öffne in Tessera **Plugins**, wähle **Mit Link hinzufügen** und füge `https://github.com/Orishas/tessera-loctek-desk`
   ein.
2. Füge das Plugin einem Screen hinzu. Der Screen baut einmal neu.
3. Setze im **Layout** die Kachel **Tisch** aus der Gruppe Plugins der Bibliothek.
4. Wähle im Inspektor die Abdeckung (cover) des Tischs, die Entität **Desk**. Tipp: Tippe den Namen des Tischs in die
   Liste, um sie zu finden.
5. Wähle den ESP des Tischs unter **Aus einem Gerät füllen**, um alles auf einmal zu übernehmen, oder wähle
   einzeln, was du hast. Alles ist optional, und die Kachel lässt eine Taste weg, deren Entität du nicht gewählt hast:

   | Option | Entität des LoctekMotion-Pakets |
   |---|---|
   | **Höhe** | Der Sensor **Desk height**. Ohne ihn zeigt die Kachel die Position der Abdeckung in Prozent. |
   | **Sitzen** | Die Taste **Sit** (bei manchen Bedienteilen Preset 4). |
   | **Stehen** | Die Taste **Stand** (bei manchen Bedienteilen Preset 3). |
   | **Speicher 1** | Die Taste **Preset 1**. |
   | **Speicher 2** | Die Taste **Preset 2**. |
   | **Kindersicherung** | Der Schalter **Child lock**. Die Kachel zeigt ihn als Taste **Sperre**. |
   | **Alarm** | Der Schalter **Alarm off**, der die Alarmtaste des Bedienteils drei Sekunden lang hält, oder die Taste **Alarm**, die sie einmal drückt. |
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

Wo nicht alle Reihen passen, werden die Tasten niedriger und die Höhe kleiner, bevor eine Reihe wegfällt: Eine Kachel
in 2 x 2 auf einem 4"-Screen zeigt so ihre drei Reihen. Auf einem kleinen Screen wie dem CYD zeigt eine Kachel, die
breiter als hoch ist, links die Höhe und rechts eine Reihe Tasten, wenn unter der Höhe keine Reihe passt. Wo gar keine
Tasten passen, zeigt die Kachel nur die Höhe, und ein Tippen schickt den Tisch ans andere Ende, wie bei 1 x 1.

Eine Taste wird nie gequetscht: Eine Reihe, die auf einen kleinen Screen nicht passt, entfällt ganz.

Die Tasten **Sitzen** und **Stehen** tragen ein Symbol, einen Sitz (`mdi:seat`) und einen Menschen mit erhobenen
Händen (`mdi:human-handsup`), wie Hoch, Stopp, Runter, Sperre und Alarm ihres tragen; das Wort steht unter dem Symbol,
wo die Taste hoch genug ist. Die Tasten 1 und 2 tragen ihre Zahl.

Fährt der Tisch, werden die Höhe und die Taste **Stopp** in der Akzentfarbe gezeichnet.

## Wo es ausprobiert wurde

| Screen | Wie |
|---|---|
| Guition 4" (480 x 480) | Auf dem echten Screen, mit einem Flexispot-Tisch und dem LoctekMotion-Paket. |
| CYD 2,8" (320 x 240) | Nur im Vorschau-Werkzeug. |
| Waveshare 4,3" hochkant (480 x 800) | Nur im Vorschau-Werkzeug. |
| Guition JC8012P4A1 10,1" (800 x 1280) | Nur im Vorschau-Werkzeug. |

Das Vorschau-Werkzeug ist `tools/preview.py` von tessera-plugins: Es baut den Code des Plugins für die Größe des
Screens und zeichnet jede Größe der Kachel, aber kein Finger berührt sie, und kein Tisch fährt.

## Gut zu wissen

- Die Taste **Alarm** drückt die Alarmtaste am Bedienteil des Tischs, die zu seiner Erinnerung ans Aufstehen gehört.
  Mit dem Schalter **Alarm off** hält die Kachel diese Taste drei Sekunden lang, wie das Paket es tut; mit der Taste
  **Alarm** drückt sie sie einmal. Was die Taste an deinem Tisch tut, steht in der Anleitung des Tischs.

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
- Die Symbole von Sitzen und Stehen sind Material Design Icons (Apache 2.0), dieselben, die Home Assistant nutzt.
  `tools/make_icons.py` macht `components/loctek_desk/icons.h` aus `tools/svg/`.
- Dieses Plugin steht in keiner Verbindung zu Loctek, Flexispot oder dem Projekt LoctekMotion_IoT.
