#pragma once
// The icons' masks at another size. No LVGL, no Tessera, so the host tests can run it: sh tests/run.sh
#include <cstdint>

namespace esphome::loctek_desk {

// The largest an icon is drawn, in pixels a side: twice the size it is kept at. Larger only blurs it, and its mask
// (side x side bytes) would grow without end on a large key.
static constexpr int ICON_MOST = 128;
// The mask src, src_side pixels a side, made side x side into dst (area mean when it shrinks; when it grows, each
// pixel blends the source pixels it covers: desk_panel's algorithm).
void resample(const uint8_t *src, int src_side, int side, uint8_t *dst);

}  // namespace esphome::loctek_desk
