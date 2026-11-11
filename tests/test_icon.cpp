// Host tests of icon.h: sh tests/run.sh
#include <cassert>
#include <cstdint>
#include <vector>
#include "icon.h"

using namespace esphome::loctek_desk;

// A uniform mask stays uniform, shrunk and grown.
static void test_uniform() {
  std::vector<uint8_t> src(64 * 64, 200);
  for (int side : {24, 64, 128}) {
    std::vector<uint8_t> dst(size_t(side) * side, 0);
    resample(src.data(), 64, side, dst.data());
    for (uint8_t v : dst) assert(v == 200);
  }
}

// Half 255 and half 0 keeps its mean.
static void test_half_mean() {
  std::vector<uint8_t> src(64 * 64, 0);
  for (int y = 0; y < 64; ++y)
    for (int x = 0; x < 32; ++x) src[y * 64 + x] = 255;
  std::vector<uint8_t> dst(32 * 32, 0);
  resample(src.data(), 64, 32, dst.data());
  long sum = 0;
  for (uint8_t v : dst) sum += v;
  const double mean = double(sum) / dst.size();
  assert(mean >= 125 && mean <= 129);  // within 2 of 127
}

// The same side copies.
static void test_same_side_copies() {
  std::vector<uint8_t> src(64 * 64);
  for (size_t i = 0; i < src.size(); ++i) src[i] = uint8_t(i * 37 + 11);
  std::vector<uint8_t> dst(64 * 64, 0);
  resample(src.data(), 64, 64, dst.data());
  assert(dst == src);
}

static void test_most() { static_assert(ICON_MOST == 128, "ICON_MOST"); }

void test_icon() {
  test_uniform();
  test_half_mean();
  test_same_side_copies();
  test_most();
}
