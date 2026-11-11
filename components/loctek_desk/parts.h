#pragma once
// LVGL parts of the tile, and a setter that leaves a part alone when nothing changes: LVGL draws a part again on every
// flag set, whether or not the value is new.
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::loctek_desk {

// A key's block: no padding, no scrolling; what lies in it is placed by hand.
inline lv_obj_t *key_block(lv_obj_t *parent) {
  lv_obj_t *b = tessera::ui::block(parent, theme::TRACK);
  lv_obj_set_style_pad_all(b, 0, 0);
  lv_obj_remove_flag(b, LV_OBJ_FLAG_SCROLLABLE);
  return b;
}

// The text's alignment, set only when it differs.
inline void set_align(lv_obj_t *o, lv_text_align_t a) {
  if (!o || lv_obj_get_style_text_align(o, LV_PART_MAIN) == a) return;
  lv_obj_set_style_text_align(o, a, 0);
}

inline void show(lv_obj_t *o, bool on) {
  if (!o || lv_obj_has_flag(o, LV_OBJ_FLAG_HIDDEN) == !on) return;
  if (on) lv_obj_remove_flag(o, LV_OBJ_FLAG_HIDDEN);
  else lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
}

}  // namespace esphome::loctek_desk
