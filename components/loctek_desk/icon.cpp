#include "icon.h"
#include <algorithm>
#include <cmath>

namespace esphome::loctek_desk {

// Each pixel of dst is the mean of the source pixels it covers, weighed by how much of each it covers; when it grows,
// a pixel covers parts of one or two source pixels a direction and blends them. desk_panel's algorithm, with the
// source's side as a parameter. LVGL then draws the mask as it is, without a transform of its own.
void resample(const uint8_t *src, int src_side, int side, uint8_t *dst) {
  for (int y = 0; y < side; ++y)
    for (int x = 0; x < side; ++x) {
      const float x0 = float(x) * src_side / side, x1 = float(x + 1) * src_side / side;
      const float y0 = float(y) * src_side / side, y1 = float(y + 1) * src_side / side;
      float sum = 0, area = 0;
      for (int sy = int(y0); sy < int(std::ceil(y1)) && sy < src_side; ++sy)
        for (int sx = int(x0); sx < int(std::ceil(x1)) && sx < src_side; ++sx) {
          const float w = (std::min(x1, float(sx + 1)) - std::max(x0, float(sx))) *
                          (std::min(y1, float(sy + 1)) - std::max(y0, float(sy)));
          sum += w * src[sy * src_side + sx];
          area += w;
        }
      dst[y * side + x] = area > 0 ? uint8_t(sum / area + 0.5f) : 0;
    }
}

}  // namespace esphome::loctek_desk
