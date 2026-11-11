#include "model.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace esphome {
namespace loctek_desk {

bool usable(const char *state) {
  return state && *state && strcmp(state, "unavailable") != 0 && strcmp(state, "unknown") != 0;
}

bool parse_number(const char *state, float *out) {
  if (!state || !*state) return false;
  char buf[32];
  if (strlen(state) >= sizeof buf) return false;
  strcpy(buf, state);
  for (char *p = buf; *p; ++p)
    if (*p == ',') *p = '.';
  char *end = nullptr;
  float v = strtof(buf, &end);
  if (end == buf || *end != 0) return false;
  if (out) *out = v;
  return true;
}

bool height_text(const char *state, char *out, size_t size) {
  if (!out || size == 0) return false;
  out[0] = 0;
  float v;
  if (!parse_number(state, &v)) return false;
  int n = snprintf(out, size, "%.1f", v);
  if (n < 0 || size_t(n) >= size) {
    out[0] = 0;
    return false;
  }
  return true;
}

int clamp_position(int position) { return position < 0 ? 0 : position > 100 ? 100 : position; }

Block choose_block(bool height_filled, bool height_is_number, bool has_position) {
  if (height_filled && height_is_number) return Block::HEIGHT;
  if (has_position) return Block::POSITION;
  return height_filled ? Block::DASH : Block::ICON;
}

void Motion::feed(float value, uint32_t now_ms) {
  if (seen_ && value != last_) {
    changed_ = true;
    changed_ms_ = now_ms;
  }
  last_ = value;
  seen_ = true;
}

bool Motion::moving(uint32_t now_ms) const { return changed_ && now_ms - changed_ms_ < MOVING_MS; }

}  // namespace loctek_desk
}  // namespace esphome
