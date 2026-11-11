#pragma once
#include "esphome/components/smart_display/plugin_api.h"
#include "model.h"

namespace esphome::loctek_desk {

// The desk tile: the height (or the position) on top or at the left, a bar of the position under it, and the keys the
// options give. The decisions are model.h's; this draws them and sends what a tap asks for. A tap writes no state: the
// tile paints from on_state, Motion and Feedback only.
class DeskTile : public tessera::Tile {
 public:
  explicit DeskTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override;
  void on_state(JsonObjectConst data) override;
  void on_tick(uint32_t epoch) override { paint(); }
  void on_theme() override { paint(); }
  void on_tap() override;

 private:
  void paint();
  void paint_block(bool usable, bool moving, const char *note);
  void paint_key(int slot, Look look);
  // The entity a key acts on: the cover for UP, STOP and DOWN, else its option's.
  const tessera::Text &entity_of(Key k) const;
  // Whether a key is drawn usable: the cover usable, and the key's own entity (UP, STOP, DOWN have none).
  bool key_takes(Key k) const;

  const tessera::Plugin *plugin_;
  tessera::Text entity_, name_;
  tessera::Text height_;               // the option height
  tessera::Text option_[KEYS];         // a key's own entity, "" for UP, STOP, DOWN and an option not filled in
  Have have_{};
  Layout layout_{};

  // From on_state.
  bool has_state_ = false, cover_usable_ = false, wrong_ = false;
  bool has_position_ = false;
  int position_ = 0;
  bool height_filled_ = false, height_number_ = false;
  char height_text_[16]{};
  char unit_[16]{};
  bool key_ok_[KEYS]{};                // the key's entity is under `entities` and key_usable()
  bool lock_on_ = false;
  Motion motion_;
  Feedback feedback_;

  // Parts.
  lv_obj_t *value_{}, *unit_label_{}, *track_{}, *fill_{}, *note_{};
  lv_obj_t *key_[KEYS]{}, *key_main_[KEYS]{}, *key_word_[KEYS]{};   // by slot
};

}  // namespace esphome::loctek_desk
