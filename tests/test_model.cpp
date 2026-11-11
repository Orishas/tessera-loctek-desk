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

int main() {
  test_usable();
  test_height_text();
  test_position();
  test_block();
  test_motion();
  std::puts("ok");
  return 0;
}
