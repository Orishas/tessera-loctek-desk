// The pure logic of the loctek_desk tile: what its data says. No LVGL, no Tessera, so the host tests can run it:
// sh tests/run.sh
#pragma once
#include <cstddef>
#include <cstdint>

namespace esphome {
namespace loctek_desk {

// How long after its last change the desk still counts as moving.
static constexpr uint32_t MOVING_MS = 2000;

// A state that carries a value: not null, not "", not "unavailable", not "unknown".
bool usable(const char *state);
// The state as a number. A comma counts as a point. False for text that is no number or has anything after it.
bool parse_number(const char *state, float *out);
// One decimal, no unit: 73.5 -> "73.5", 114 -> "114.0". False (and out = "") when the state is no number.
bool height_text(const char *state, char *out, size_t size);
// 0 to 100.
int clamp_position(int position);

// What the height block shows.
enum class Block { HEIGHT, POSITION, DASH, ICON };
// HEIGHT: option filled and its state a number. POSITION: no usable height, but a position.
// DASH: the height option is filled, its state is no number, and there is no position. ICON: neither.
Block choose_block(bool height_filled, bool height_is_number, bool has_position);

// Whether the desk moves: its value changed within MOVING_MS. The first value is no change.
struct Motion {
  void feed(float value, uint32_t now_ms);
  bool moving(uint32_t now_ms) const;

 private:
  float last_ = 0;
  uint32_t changed_ms_ = 0;
  bool seen_ = false, changed_ = false;
};

// How long a sent key shows PRESSED, and a key that could not send shows ALERT.
static constexpr uint32_t PRESSED_MS = 1000, FAILED_MS = 2000;

// The keys of the tile, in the order of their groups.
enum class Key : int8_t { NONE = -1, SIT, STAND, UP, STOP, DOWN, PRESET_1, PRESET_2, LOCK, ALARM, COUNT };
static constexpr int KEYS = int(Key::COUNT);
struct Rect {
  int x, y, w, h;
};
struct Slot {
  Key key = Key::NONE;
  Rect r{};
};
struct Layout {
  Rect block{};
  Slot slots[KEYS];
  int count = 0;
};

// Which keys exist: its option is filled in; UP, STOP and DOWN while the option manual is on.
struct Have {
  bool key[KEYS]{};
  int columns = 1, rows = 1;
};
// The height block and the keys' places inside w x h. min_key is ui::mm(7).
// Groups {SIT, STAND}, {UP, STOP, DOWN}, {PRESET_1, PRESET_2}, {LOCK, ALARM}; a group without a key does not exist.
// The size offers rows: 1x1 none, 2x1 one, 1x2 two, 2x2 three (groups 3 and 4 then share one row if all four exist).
// 2x1: block left, row right, side by side. Else the block on top and the rows under it. A row too low or whose
// keys would be narrower than min_key is not drawn; with no row the block is the whole tile.
Layout lay_out(int w, int h, const Have &have, int gap, int min_key);
// The key at a point. NONE between keys and on the block.
Key hit(const Layout &l, int x, int y);

// The action a key sends. entity is the key's own (for ALARM its domain chooses). nullptr for NONE.
const char *service_of(Key key, const char *entity);
// A tap on a tile without key rows: STOP while moving; else STAND under 50 %, SIT from 50 %;
// NONE without a position or when that key does not exist.
Key small_tap(const Have &have, bool moving, bool has_position, int position);
// Whether a tap may send: the cover is usable, and the key's own entity is (UP, STOP, DOWN have none of their own).
bool may_send(Key key, bool cover_usable, bool key_entity_usable);

// How a key is drawn.
enum class Look { NORMAL, ACCENT, BUSY, PRESSED, ALERT, MUTED };
// The last tap: which key, and whether it was sent.
struct Feedback {
  Key key = Key::NONE;
  bool failed = false;
  uint32_t at_ms = 0;
  // Replaces the one before.
  void set(Key k, bool sent, uint32_t now_ms);
  // Whether it still shows: PRESSED_MS when sent, FAILED_MS when not. Survives a wrap of the millisecond clock.
  bool active(uint32_t now_ms) const;
};
// MUTED: the cover or the key's entity is not usable. ALERT / PRESSED: feedback for this key is active.
// ACCENT: STOP while moving. BUSY: LOCK while its switch is on. Else NORMAL. In that order.
Look look_of(Key key, bool usable, bool moving, bool lock_on, const Feedback &feedback, uint32_t now_ms);

}  // namespace loctek_desk
}  // namespace esphome
