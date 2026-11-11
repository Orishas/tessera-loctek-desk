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

}  // namespace loctek_desk
}  // namespace esphome
