// Host tests of model.h: sh tests/run.sh
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include "model.h"

using namespace esphome::loctek_desk;

static void test_usable() {
  for (const char *s : {"open", "closed", "73.5", "0"}) assert(usable(s));
  for (const char *s : {"unavailable", "unknown", ""}) assert(!usable(s));
  assert(!usable(nullptr));
}
static void test_key_usable() {
  for (const char *s : {"unknown", "2026-11-01T11:07:37+00:00", "on", "off"}) assert(key_usable(s));
  for (const char *s : {"unavailable", ""}) assert(!key_usable(s));
  assert(!key_usable(nullptr));
}
static void test_height_text() {
  char t[12];
  assert(height_text("73.5", t, sizeof t) && !strcmp(t, "73.5"));
  assert(height_text("114", t, sizeof t) && !strcmp(t, "114.0"));
  assert(height_text("73.50000", t, sizeof t) && !strcmp(t, "73.5"));
  assert(height_text("73,5", t, sizeof t) && !strcmp(t, "73.5"));       // Review Focus 1
  assert(height_text("28.9", t, sizeof t) && !strcmp(t, "28.9"));       // inches: the same rule
  for (const char *s : {"unavailable", "unknown", "", "abc", "73.5cm", "nan", "inf", "-inf", "1e999", "0x10", " 73"}) {
    assert(!height_text(s, t, sizeof t) && t[0] == 0);
  }
}
static void test_position() {
  assert(clamp_position(-5) == 0 && clamp_position(50) == 50 && clamp_position(140) == 100);
}
static void test_block() {
  assert(choose_block(true, true, true) == Block::HEIGHT);
  assert(choose_block(true, false, true) == Block::POSITION);   // after the desk's ESP restarted
  assert(choose_block(false, false, true) == Block::POSITION);
  assert(choose_block(true, false, false) == Block::DASH);
  assert(choose_block(false, false, false) == Block::ICON);
}
static void test_motion() {
  Motion m;
  assert(!m.moving(0));
  m.feed(73.5f, 1000);
  assert(!m.moving(1000));                 // the first value is no change
  m.feed(74.1f, 2000);
  assert(m.moving(2000) && m.moving(3999));
  assert(!m.moving(4000));                 // Review Focus 2: the values stopped arriving
  m.feed(74.1f, 4500);
  assert(!m.moving(4500));                 // the same value again is no change
  m.feed(73.0f, 5000);
  assert(m.moving(5000));
  Motion n;
  n.feed(73.5f, 1000);
  n.feed(NAN, 1100);
  n.feed(INFINITY, 1200);
  assert(!n.moving(1200));                 // a non-finite value changes nothing
  n.feed(73.5f, 1300);
  assert(!n.moving(1300));
}

static Have all(int c, int r) { Have h; for (bool &k : h.key) k = true; h.columns = c; h.rows = r; return h; }
static const Slot *find(const Layout &l, Key k) {
  for (int i = 0; i < l.count; ++i) if (l.slots[i].key == k) return &l.slots[i];
  return nullptr;
}
static void inside_and_apart(const Layout &l, int w, int h, int min_key) {
  for (int i = 0; i < l.count; ++i) {
    const Rect a = l.slots[i].r;
    assert(a.x >= 0 && a.y >= 0 && a.x + a.w <= w && a.y + a.h <= h && a.w >= min_key && a.h >= min_key);
    for (int j = i + 1; j < l.count; ++j) {
      const Rect b = l.slots[j].r;
      assert(a.x + a.w <= b.x || b.x + b.w <= a.x || a.y + a.h <= b.y || b.y + b.h <= a.y);
    }
  }
}
static void test_layout_small() {
  const Layout l = lay_out(200, 200, all(1, 1), 8, 40);
  assert(l.count == 0 && l.block.w == 200 && l.block.h == 200);
}
static void test_layout_wide() {
  const Layout l = lay_out(408, 200, all(2, 1), 8, 40);
  assert(l.count == 2 && find(l, Key::SIT) && find(l, Key::STAND));       // the first group alone
  assert(l.block.x == 0 && l.block.w == 200 && find(l, Key::SIT)->r.x == 208);
  inside_and_apart(l, 408, 200, 40);
}
static void test_layout_wide_without_sit_stand() {
  Have h = all(2, 1); h.key[int(Key::SIT)] = h.key[int(Key::STAND)] = false;
  const Layout l = lay_out(408, 200, h, 8, 40);
  assert(l.count == 3 && find(l, Key::UP) && find(l, Key::STOP) && find(l, Key::DOWN));
}
static void test_layout_tall() {
  const Layout l = lay_out(200, 408, all(1, 2), 8, 40);
  assert(l.count == 5 && !find(l, Key::PRESET_1) && !find(l, Key::LOCK));  // two rows: groups 1 and 2
  inside_and_apart(l, 200, 408, 40);
}
static void test_layout_large_all() {
  const Layout l = lay_out(408, 408, all(2, 2), 8, 40);
  assert(l.count == 9);                                                     // groups 3 and 4 share the third row
  assert(find(l, Key::PRESET_1)->r.y == find(l, Key::ALARM)->r.y);
  assert(l.block.h >= 40);
  inside_and_apart(l, 408, 408, 40);
}
static void test_layout_large_three_groups() {
  Have h = all(2, 2); h.key[int(Key::LOCK)] = h.key[int(Key::ALARM)] = false;
  const Layout l = lay_out(408, 408, h, 8, 40);
  assert(l.count == 7 && find(l, Key::PRESET_2));
}
static void test_layout_manual_off_only() {
  Have h; h.columns = h.rows = 2;                                           // nothing filled in, manual off
  const Layout l = lay_out(408, 408, h, 8, 40);
  assert(l.count == 0 && l.block.h == 408);
}
static void test_layout_smallest_glass() {                                  // Review Focus 4: the CYD, 2x2
  const Layout l = lay_out(220, 150, all(2, 2), 6, 56);
  inside_and_apart(l, 220, 150, 56);
  assert(l.block.h >= 56);
  assert(l.count <= 2);                                                     // one row at most fits; never a squeezed key
}
static void test_layout_flat_falls_back_to_wide() {      // the CYD's 2x2
  const Layout l = lay_out(302, 110, all(2, 2), 8, 39);
  assert(l.count == 2 && find(l, Key::SIT) && find(l, Key::STAND));
  assert(l.block.x == 0 && l.block.w == 147 && l.block.h == 110 && find(l, Key::SIT)->r.x == 155);
  inside_and_apart(l, 302, 110, 39);
}
static void test_layout_flat_too_narrow() {              // the CYD's 1x2: nothing fits, the tap of a small tile works
  const Layout l = lay_out(148, 110, all(1, 2), 8, 39);
  assert(l.count == 0 && l.block.w == 148 && l.block.h == 110);
}
static void test_hit() {
  const Layout l = lay_out(408, 408, all(2, 2), 8, 40);
  const Rect r = find(l, Key::STOP)->r;
  assert(hit(l, r.x, r.y) == Key::STOP && hit(l, r.x + r.w - 1, r.y + r.h - 1) == Key::STOP);
  assert(hit(l, r.x + r.w, r.y) == Key::NONE);                              // the gap
  assert(hit(l, 10, 10) == Key::NONE && hit(l, -1, -1) == Key::NONE);       // the block; a finger not known
}
static void test_services() {
  assert(!strcmp(service_of(Key::UP, "cover.desk"), "cover.open_cover"));
  assert(!strcmp(service_of(Key::DOWN, "cover.desk"), "cover.close_cover"));
  assert(!strcmp(service_of(Key::STOP, "cover.desk"), "cover.stop_cover"));
  for (Key k : {Key::SIT, Key::STAND, Key::PRESET_1, Key::PRESET_2})
    assert(!strcmp(service_of(k, "button.x"), "button.press"));
  assert(!strcmp(service_of(Key::LOCK, "switch.child_lock"), "switch.turn_on"));
  assert(!strcmp(service_of(Key::ALARM, "button.alarm"), "button.press"));
  assert(!strcmp(service_of(Key::ALARM, "switch.alarm_off"), "switch.turn_on"));   // Review Focus 5
  assert(service_of(Key::NONE, "") == nullptr);
}
static void test_small_tap() {
  const Have h = all(1, 1);
  assert(small_tap(h, true, true, 10) == Key::STOP);
  assert(small_tap(h, false, true, 49) == Key::STAND && small_tap(h, false, true, 50) == Key::SIT);
  assert(small_tap(h, false, false, 0) == Key::NONE);
  Have none; assert(small_tap(none, false, true, 10) == Key::NONE);
}
static void test_may_send() {                                               // Review Focus 3
  assert(may_send(Key::UP, true, false) && may_send(Key::SIT, true, true));
  assert(!may_send(Key::UP, false, false) && !may_send(Key::SIT, false, true));
  assert(!may_send(Key::SIT, true, false) && !may_send(Key::NONE, true, true));
}
static void test_looks() {
  Feedback f;
  assert(look_of(Key::STOP, true, true, false, f, 0) == Look::ACCENT);
  assert(look_of(Key::STOP, true, false, false, f, 0) == Look::NORMAL);
  assert(look_of(Key::LOCK, true, false, true, f, 0) == Look::BUSY);
  assert(look_of(Key::SIT, false, false, false, f, 0) == Look::MUTED);
  f.set(Key::SIT, true, 1000);
  assert(look_of(Key::SIT, true, false, false, f, 1500) == Look::PRESSED);
  assert(look_of(Key::SIT, true, false, false, f, 2000) == Look::NORMAL);
  assert(look_of(Key::STAND, true, false, false, f, 1500) == Look::NORMAL);
  f.set(Key::UP, false, 3000);                                              // not sent; replaces the one before
  assert(look_of(Key::UP, true, false, false, f, 4999) == Look::ALERT);
  assert(look_of(Key::UP, true, false, false, f, 5000) == Look::NORMAL);
}

int main() {
  test_usable();
  test_key_usable();
  test_height_text();
  test_position();
  test_block();
  test_motion();
  test_layout_small();
  test_layout_wide();
  test_layout_wide_without_sit_stand();
  test_layout_tall();
  test_layout_large_all();
  test_layout_large_three_groups();
  test_layout_manual_off_only();
  test_layout_smallest_glass();
  test_layout_flat_falls_back_to_wide();
  test_layout_flat_too_narrow();
  test_hit();
  test_services();
  test_small_tap();
  test_may_send();
  test_looks();
  std::puts("ok");
  return 0;
}
