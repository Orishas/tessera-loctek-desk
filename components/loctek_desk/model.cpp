#include "model.h"

#include <cctype>
#include <cmath>
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
  if (isspace(static_cast<unsigned char>(buf[0]))) return false;
  for (char *p = buf; *p; ++p) {
    if (*p == 'x' || *p == 'X') return false;  // strtof reads hex
    if (*p == ',') *p = '.';
  }
  char *end = nullptr;
  float v = strtof(buf, &end);
  if (end == buf || *end != 0 || !std::isfinite(v)) return false;
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

bool key_usable(const char *state) { return state && *state && strcmp(state, "unavailable") != 0; }

int clamp_position(int position) { return position < 0 ? 0 : position > 100 ? 100 : position; }

int position_of(float value) {
  if (!(value > 0)) return 0;  // also NaN
  if (value > 100) return 100;
  return int(value + 0.5f);
}

bool cover_moving(const char *state) {
  return state && (strcmp(state, "opening") == 0 || strcmp(state, "closing") == 0);
}

Block choose_block(bool height_filled, bool height_is_number, bool has_position) {
  if (height_filled && height_is_number) return Block::HEIGHT;
  if (has_position) return Block::POSITION;
  return height_filled ? Block::DASH : Block::ICON;
}

void Motion::feed(float value, uint32_t now_ms) {
  if (!std::isfinite(value)) return;  // NaN is unequal to everything: it would read as moving on every feed
  if (seen_ && value != last_) {
    changed_ = true;
    changed_ms_ = now_ms;
  }
  last_ = value;
  seen_ = true;
}

void Motion::reset() { seen_ = false; }

bool Motion::moving(uint32_t now_ms) const { return changed_ && now_ms - changed_ms_ < MOVING_MS; }

namespace {
struct Row {
  Key keys[4];
  int n = 0;
};
const Key GROUPS[4][3] = {{Key::SIT, Key::STAND, Key::NONE},
                          {Key::UP, Key::STOP, Key::DOWN},
                          {Key::PRESET_1, Key::PRESET_2, Key::NONE},
                          {Key::LOCK, Key::ALARM, Key::NONE}};
}  // namespace

Layout lay_out(int w, int h, const Have &have, int gap, int min_key) {
  Layout l;
  l.block = {0, 0, w, h};
  Row groups[4];
  int ng = 0;
  for (const auto &g : GROUPS) {
    Row r;
    for (Key k : g)
      if (k != Key::NONE && have.key[int(k)]) r.keys[r.n++] = k;
    if (r.n) groups[ng++] = r;
  }
  bool wide = have.columns >= 2 && have.rows < 2;
  int offered = have.rows >= 2 ? (have.columns >= 2 ? 3 : 2) : (have.columns >= 2 ? 1 : 0);
  Row rows[4];
  int nrows = 0;
  if (offered == 3 && ng == 4) {  // groups 3 and 4 share a row
    rows[0] = groups[0];
    rows[1] = groups[1];
    rows[2] = groups[2];
    for (int i = 0; i < groups[3].n; ++i) rows[2].keys[rows[2].n++] = groups[3].keys[i];
    nrows = 3;
  } else {
    for (int i = 0; i < offered && i < ng; ++i) rows[nrows++] = groups[i];
  }
  int row_w = wide ? (w - gap) / 2 : w;
  const auto keep_wide_enough = [&]() {  // a row of too narrow keys is dropped whole
    int kept = 0;
    for (int i = 0; i < nrows; ++i) {
      const int m = rows[i].n;
      if ((row_w - (m - 1) * gap) / m >= min_key) rows[kept++] = rows[i];
    }
    nrows = kept;
  };
  keep_wide_enough();
  int rh = h;
  if (!wide) {
    while (nrows > 0) {
      rh = (h - nrows * gap) / (nrows + 2);
      if (rh >= min_key) break;
      --nrows;
    }
    // A tile wider than high that the stacked rule leaves without a row: the wide rule, with the first group.
    if (nrows == 0 && offered > 0 && ng > 0 && w > h) {
      wide = true;
      row_w = (w - gap) / 2;
      rows[0] = groups[0];
      nrows = 1;
      keep_wide_enough();
      rh = h;
    }
  }
  if (wide) {
    if (nrows > 1) nrows = 1;
    if (h < min_key) nrows = 0;
  }
  if (nrows == 0) return l;
  int x0, y0;
  if (wide) {
    l.block = {0, 0, row_w, h};
    x0 = w - row_w;
    y0 = 0;
  } else {
    l.block = {0, 0, w, h - nrows * (rh + gap)};
    x0 = 0;
    y0 = l.block.h + gap;
  }
  for (int i = 0; i < nrows; ++i) {
    const int m = rows[i].n;
    const int kw = (row_w - (m - 1) * gap) / m;
    for (int j = 0; j < m; ++j)
      l.slots[l.count++] = {rows[i].keys[j], {x0 + j * (kw + gap), y0 + i * (rh + gap), kw, rh}};
  }
  return l;
}

Key hit(const Layout &l, int x, int y) {
  for (int i = 0; i < l.count; ++i) {
    const Rect &r = l.slots[i].r;
    if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return l.slots[i].key;
  }
  return Key::NONE;
}

const char *service_of(Key key, const char *entity) {
  switch (key) {
    case Key::UP: return "cover.open_cover";
    case Key::DOWN: return "cover.close_cover";
    case Key::STOP: return "cover.stop_cover";
    case Key::SIT:
    case Key::STAND:
    case Key::PRESET_1:
    case Key::PRESET_2: return "button.press";
    case Key::LOCK: return "switch.turn_on";
    case Key::ALARM: return entity && strncmp(entity, "switch.", 7) == 0 ? "switch.turn_on" : "button.press";
    default: return nullptr;
  }
}

Key small_tap(const Have &have, bool moving, bool has_position, int position) {
  if (moving) return Key::STOP;  // its entity is the cover, which is always there
  if (!has_position) return Key::NONE;
  const Key k = clamp_position(position) < 50 ? Key::STAND : Key::SIT;
  return have.key[int(k)] ? k : Key::NONE;
}

bool may_send(Key key, bool cover_usable, bool key_entity_usable) {
  if (key == Key::NONE || !cover_usable) return false;
  if (key == Key::UP || key == Key::STOP || key == Key::DOWN) return true;
  return key_entity_usable;
}

void Feedback::set(Key k, bool sent, uint32_t now_ms) {
  key = k;
  failed = !sent;
  at_ms = now_ms;
}

bool Feedback::active(uint32_t now_ms) const {
  return key != Key::NONE && now_ms - at_ms < (failed ? FAILED_MS : PRESSED_MS);
}

Look look_of(Key key, bool usable, bool moving, bool lock_on, const Feedback &feedback, uint32_t now_ms) {
  if (!usable) return Look::MUTED;
  if (feedback.key == key && feedback.active(now_ms)) return feedback.failed ? Look::ALERT : Look::PRESSED;
  if (key == Key::STOP && moving) return Look::ACCENT;
  if (key == Key::LOCK && lock_on) return Look::BUSY;
  return Look::NORMAL;
}

}  // namespace loctek_desk
}  // namespace esphome
