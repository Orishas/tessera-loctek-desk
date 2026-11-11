# Loctek desk

A Loctek Motion or Flexispot desk on the screen. The tile shows the desk's height with a bar for its position. Its keys
send the desk to sitting or standing, move it up or down, stop it and call the presets. You choose which keys the tile
has, and it draws only those.

> The tile works with the entities of the [LoctekMotion_IoT](https://github.com/iMicknl/LoctekMotion_IoT) package: an
> ESP32 on the desk's controller, set up in Home Assistant. Set that up first.

## Set up

1. In Tessera, open **Plugins**, choose **Add with a link** and paste `https://github.com/Orishas/tessera-loctek-desk`.
2. Add the plugin to a screen. The screen builds once.
3. In **Layout**, place the **Desk** tile from the library's Plugins group.
4. In the inspector, choose the desk's cover (the entity **Desk**). Tip: type the desk's name in the list to find it.
5. Choose what you have. All of it is optional, and the tile leaves out a key whose entity you did not choose:

   | Option | Entity of the LoctekMotion package |
   |---|---|
   | **Height** | The sensor **Desk height**. Without it the tile shows the cover's position in percent. |
   | **Sit** | The button **Sit** (Preset 4 on some panels). |
   | **Stand** | The button **Stand** (Preset 3 on some panels). |
   | **Preset 1** | The button **Preset 1**. |
   | **Preset 2** | The button **Preset 2**. |
   | **Child lock** | The switch **Child lock**. The tile shows it as the key **Lock**. |
   | **Alarm** | The button **Alarm**, or the switch **Alarm off** if that is what your package has. |
   | **Manual keys** | On by default. The keys **Up**, **Stop** and **Down**. |

## Sizes

| Size | What it shows |
|---|---|
| 1 x 1 | The height alone. A tap sends the desk to the other end: to Stand while it is under 50 %, else to Sit. While the desk moves, a tap stops it. A tap does nothing when the needed key has no entity or the cover gives no position. |
| 2 x 1 | The height on the left, one row of keys on the right. |
| 1 x 2 | The height on top, two rows of keys under it. |
| 2 x 2 | The height on top, three rows of keys under it. |

The keys come in four groups, in this order: Sit and Stand; Up, Stop and Down; 1 and 2; Lock and Alarm. A group
without a key does not exist. Each size fills its rows with the groups in that order, and the groups that find no
row are not drawn.

A tile that is much wider than high, such as a 2 x 2 or 1 x 2 tile on a small screen like the CYD, shows the height on
the left and one row of keys on the right when no row fits under the height. Where no keys fit at all, the tile shows
the height alone, and a tap sends the desk to the other end, as on 1 x 1.

A key is never squeezed: a row that does not fit a small screen is left out as a whole.

While the desk moves, the height and the **Stop** key turn to the accent colour.

## Good to know

- **Up** and **Down** start a travel that runs until you press **Stop**, the other direction, or the end of the range.
  The package enforces the range. A plugin's tile gets taps, not holds, so a key cannot move the desk only while you
  hold it.
- The **Lock** key does not show whether the desk is locked: the controller does not say. It presses the child lock
  for five seconds, and while the switch holds, the key is drawn on an amber ground.
- A key is muted and takes no tap only while its entity is `unavailable`. A button nobody has pressed yet (state
  `unknown`) works.
- When Home Assistant cannot be reached for an action, the key shows the alert colour for two seconds.
- The height shows only after the desk's controller woke. The package wakes it ten seconds after its ESP started. Until
  then the tile shows the position in percent. A height in inches is shown in inches; the tile converts nothing.
- The screen must be allowed to perform Home Assistant actions (Tessera turns it on when it adds the screen).
- On a Guition 4" turned by **Rotation** with firmware 0.60.0, a tap on a plugin's tile can be lost. Use Rotation 0°.
- This plugin is not affiliated with Loctek, Flexispot or the LoctekMotion_IoT project.
