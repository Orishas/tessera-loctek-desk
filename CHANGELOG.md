# Changelog

## 0.2.1 - 2026-11-24
- The README says how to set up the desk's side: which desks fit, the parts, the wiring, the package to flash and the
  entities it gives. It also says on which screens the tile was tried, the real one and the preview tool apart.
- Fixed: the hint of the option **Alarm** said the key sounds the desk's alarm. It presses the control panel's alarm
  key, and with the switch **Alarm off** it holds it for three seconds.
- The tile's `memory` counts the masks of Sit and Stand as a board without PSRAM keeps them: 7900 bytes, before 4300.

## 0.2.0 - 2026-11-17
- New: **Fill from a device** above the options. Choose the desk's ESP there and the tile takes its height sensor and
  its keys Sit, Stand, Preset 1, Preset 2, Child lock and Alarm at once. Each option's list also shows the fitting
  entity first, with a key for every entity.
- Needs Tessera 0.4.98 or later (plugin API 0.26).

## 0.1.0 - 2026-11-11
- The first version: a tile that shows a Loctek Motion or Flexispot desk's height and moves the desk, with the
  entities of the LoctekMotion package.
- Eight options: the height, the keys Sit, Stand, Preset 1, Preset 2, Child lock and Alarm, and the manual keys Up,
  Stop and Down. Sit and Stand carry a seat and a standing person, the other keys their icons or numbers.
- Four sizes, 1 x 1 to 2 x 2. On 1 x 1 a tap sends the desk to the other end.
