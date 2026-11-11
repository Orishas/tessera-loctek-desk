#include "desk_tile.h"
#include "icon.h"
#include "icons.h"
#include "parts.h"
#include "esphome/core/hal.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>

namespace esphome::loctek_desk {

using tessera::Font;
using tessera::Tone;
namespace ui = tessera::ui;

// Icons of Tessera's set.
static constexpr uint32_t ICON_UP = 0xF005D, ICON_DOWN = 0xF0045, ICON_STOP = 0xF04DB, ICON_LOCK = 0xF033E,
                          ICON_ALARM = 0xF0020, ICON_DESK = 0xF1239;

// What a key shows and which option names its entity, by Key.
struct KeyFace {
  uint32_t icon;        // an icon of Tessera's set; 0: a mask or a word key
  const uint8_t *mask;  // an icon of icons.h (ICON_SIZE a side), for the icons Tessera's set lacks; nullptr: none
  const char *word;     // the key of translations/*.json, part screen
  const char *option;   // the tile's option of its entity, nullptr for the cover's own keys
};
static const KeyFace FACES[KEYS] = {
    {0, ICON_SEAT, "sit", "sit"},
    {0, ICON_HUMAN_HANDSUP, "stand", "stand"},
    {ICON_UP, nullptr, "up", nullptr},
    {ICON_STOP, nullptr, "stop", nullptr},
    {ICON_DOWN, nullptr, "down", nullptr},
    {0, nullptr, "preset_1", "preset_1"},
    {0, nullptr, "preset_2", "preset_2"},
    {ICON_LOCK, nullptr, "lock", "child_lock"},
    {ICON_ALARM, nullptr, "alarm", "alarm"},
};
// Only SIT and STAND have a mask: DeskTile keeps their masks by Key.
static_assert(int(Key::SIT) < DeskTile::MASKS && int(Key::STAND) < DeskTile::MASKS, "a mask's key");

static tessera::Text copy(const char *s) { return roomy::text(std::string(s ? s : "")); }

void DeskTile::create(const tessera::TileContext &c) {
  entity_ = copy(c.entity);
  name_ = copy(c.name);
  height_ = copy(c.options["height"] | "");
  const bool manual = c.options["manual"] | true;
  for (int k = 0; k < KEYS; ++k) {
    const KeyFace &f = FACES[k];
    if (f.option) option_[k] = copy(c.options[f.option] | "");
    have_.key[k] = f.option ? !option_[k].empty() : manual;
  }
  have_.columns = c.columns;
  have_.rows = c.rows;
  // Size and options never change within one object: laid out once.
  layout_ = lay_out(c.width, c.height, have_, ui::px(8), ui::mm(7));
  tile_ = {0, 0, int(c.width), int(c.height)};

  value_ = ui::label(c.parent, Font::DIGITS, theme::INK);
  unit_label_ = ui::label(c.parent, Font::BODY, theme::INK);
  track_ = ui::block(c.parent, theme::TRACK);
  fill_ = ui::block(track_, theme::ACCENT);
  note_ = ui::text(c.parent, Font::BODY, theme::MUTED);
  lv_obj_set_style_text_align(note_, LV_TEXT_ALIGN_CENTER, 0);

  const int pad = ui::px(4);
  for (int i = 0; i < layout_.count; ++i) {
    const Slot &s = layout_.slots[i];
    const KeyFace &f = FACES[int(s.key)];
    lv_obj_t *b = key_block(c.parent);
    lv_obj_set_pos(b, s.r.x, s.r.y);
    lv_obj_set_size(b, s.r.w, s.r.h);
    key_[i] = b;
    if (f.icon || f.mask) {
      const Font font = s.r.h < ui::mm(10) ? Font::ICON_SMALL : Font::ICON;
      // A mask is as high as the icon label of that key would be, at most ICON_MOST, which bounds its bytes.
      const int ih = f.mask ? std::min(ui::line_height(font), ICON_MOST) : ui::line_height(font);
      const int wh = ui::line_height(Font::BODY);
      const char *word = plugin_->text(f.word);
      // The word under the icon on a high key, where it fits the key's width.
      const bool worded = s.r.h >= ui::mm(14) && ih + pad + wh <= s.r.h &&
                          ui::text_width(word, Font::BODY) <= s.r.w - 2 * pad;
      const int top = worded ? (s.r.h - ih - pad - wh) / 2 : (s.r.h - ih) / 2;
      if (f.mask && ih > 0) {
        // The mask at the key's size, made once: the size never changes within one object. Painted in the ink.
        const int m = int(s.key);
        mask_[m].assign(size_t(ih) * ih, 0);
        resample(f.mask, ICON_SIZE, ih, mask_[m].data());
        lv_image_dsc_t &d = image_[m];
        d = lv_image_dsc_t{};
        d.header.magic = LV_IMAGE_HEADER_MAGIC;
        d.header.cf = LV_COLOR_FORMAT_A8;
        d.header.w = ih;
        d.header.h = ih;
        d.header.stride = ih;
        d.data = mask_[m].data();
        d.data_size = mask_[m].size();
        key_main_[i] = lv_image_create(b);
        lv_obj_set_style_image_recolor_opa(key_main_[i], LV_OPA_COVER, 0);
        lv_obj_remove_flag(key_main_[i], LV_OBJ_FLAG_CLICKABLE);
        lv_image_set_src(key_main_[i], &d);
        lv_obj_set_pos(key_main_[i], (s.r.w - ih) / 2, top);
        lv_obj_set_size(key_main_[i], ih, ih);
        key_masked_[i] = true;
      } else if (f.icon) {
        key_main_[i] = ui::label(b, font, theme::INK);
        lv_obj_set_width(key_main_[i], s.r.w);
        lv_obj_set_style_text_align(key_main_[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_pos(key_main_[i], 0, top);
        ui::set_text(key_main_[i], ui::icon(f.icon));
      }
      if (worded) {
        key_word_[i] = ui::label(b, Font::BODY, theme::INK);
        lv_obj_set_width(key_word_[i], s.r.w);
        lv_obj_set_style_text_align(key_word_[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_pos(key_word_[i], 0, top + ih + pad);
        ui::set_text(key_word_[i], word);
      }
    } else {
      const char *word = plugin_->text(f.word);
      // The words of a key, smaller where the key is too narrow for them.
      const Font font = ui::text_width(word, Font::BODY_LARGE) <= s.r.w - 2 * pad ? Font::BODY_LARGE : Font::BODY;
      key_main_[i] = ui::label(b, font, theme::INK);
      lv_obj_set_width(key_main_[i], s.r.w);
      lv_obj_set_style_text_align(key_main_[i], LV_TEXT_ALIGN_CENTER, 0);
      lv_obj_set_pos(key_main_[i], 0, (s.r.h - ui::line_height(font)) / 2);
      ui::set_text(key_main_[i], word);
    }
  }
  paint();
}

void DeskTile::on_state(JsonObjectConst data) {
  const char *state = data["state"].as<const char *>();
  has_state_ = state != nullptr;
  cover_usable_ = usable(state);
  wrong_ = strcmp(data["wait"] | "", "wrong_entity") == 0;
  cover_moving_ = loctek_desk::cover_moving(state);

  JsonVariantConst p = data["attributes"]["current_position"];
  float pos = 0;
  has_position_ = p.is<float>() ? (pos = p.as<float>(), true) : parse_number(p.as<const char *>(), &pos);
  position_ = has_position_ ? position_of(pos) : 0;

  JsonObjectConst e = data["entities"];
  // An option whose entity is not under `entities` counts as not filled in.
  JsonObjectConst h = e["height"];
  height_filled_ = !height_.empty() && !h.isNull();
  const char *hs = h["state"].as<const char *>();
  height_number_ = height_filled_ && height_text(hs, height_text_, sizeof(height_text_));
  snprintf(unit_, sizeof(unit_), "%s", height_filled_ ? (h["attributes"]["unit_of_measurement"] | "") : "");

  for (int k = 0; k < KEYS; ++k) {
    const KeyFace &f = FACES[k];
    if (!f.option || option_[k].empty()) {
      key_ok_[k] = false;
      continue;
    }
    JsonObjectConst ke = e[f.option];
    key_ok_[k] = !ke.isNull() && loctek_desk::key_usable(ke["state"].as<const char *>());
  }
  lock_on_ = !option_[int(Key::LOCK)].empty() && strcmp(e["child_lock"]["state"] | "", "on") == 0;

  const uint32_t now = esphome::millis();
  float hv = 0;
  Source source = Source::NONE;
  if (height_number_ && parse_number(hs, &hv)) source = Source::HEIGHT;
  else if (has_position_) source = Source::POSITION;
  if (source != Source::NONE) {
    if (source != source_) motion_.reset();  // a height and a position are no change of each other
    source_ = source;
    motion_.feed(source == Source::HEIGHT ? hv : float(position_), now);
  }
  paint();
}

const tessera::Text &DeskTile::entity_of(Key k) const {
  return FACES[int(k)].option ? option_[int(k)] : entity_;
}

bool DeskTile::key_takes(Key k) const {
  if (!cover_usable_) return false;
  return FACES[int(k)].option ? key_ok_[int(k)] : true;
}

void DeskTile::on_tap() {
  if (wrong_) return;  // muted as the paint shows it
  const tessera::Point p = tessera::tap_point();
  const uint32_t now = esphome::millis();
  const Key k = layout_.count ? hit(layout_, p.x, p.y)
                              : small_tap(have_, moving(now), has_position_, position_);
  if (k == Key::NONE) return;
  const bool own = FACES[int(k)].option != nullptr;
  if (!may_send(k, cover_usable_, own ? key_ok_[int(k)] : true)) return;
  const std::string entity = roomy::plain(entity_of(k));
  const char *service = service_of(k, entity.c_str());
  if (!service) return;
  const bool sent = tessera::action(service, entity);
  feedback_.set(k, sent, now);
  paint();
}

void DeskTile::paint() {
  const uint32_t now = esphome::millis();
  const bool moves = moving(now);
  // No entity yet, or one of another domain: the desk icon and the line to choose it, no keys.
  const bool chosen = has_state_ && !wrong_;
  const char *note = nullptr;
  if (!chosen) note = plugin_->text("choose_desk");
  else if (!cover_usable_) note = plugin_->text("unavailable");
  paint_block(cover_usable_ && chosen, moves, chosen, note);
  for (int i = 0; i < layout_.count; ++i) {
    const Key k = layout_.slots[i].key;
    show(key_[i], chosen);
    paint_key(i, look_of(k, key_takes(k) && chosen, moves, lock_on_, feedback_, now));
  }
}

// The height, the position or the desk's icon, centred in the block, and under it the bar or the note.
void DeskTile::paint_block(bool usable, bool moving, bool chosen, const char *note) {
  const Rect &b = chosen ? layout_.block : tile_;  // no keys while no desk is chosen
  const int gap = ui::px(4), bar_h = ui::px(6);
  const Block kind = chosen ? choose_block(height_filled_, height_number_, has_position_) : Block::ICON;

  std::string main, side;
  char pos[8];
  switch (kind) {
    case Block::HEIGHT:
      main = height_text_;
      side = unit_;
      break;
    case Block::POSITION:
      snprintf(pos, sizeof(pos), "%d", position_);
      main = pos;
      side = "%";
      break;
    case Block::DASH:
      main = "\xE2\x80\x93";   // an en dash
      break;
    case Block::ICON:
      main = ui::icon(ICON_DESK);
      if (chosen) side = roomy::plain(name_);
      break;
  }
  const bool icon = kind == Block::ICON;

  // Under it: the note when there is one, else the bar of the position.
  const bool has_note = note != nullptr;
  const bool bar = !has_note && has_position_ && chosen;
  const int body_h = ui::line_height(Font::BODY);
  const int note_lines = has_note && ui::text_width(note, Font::BODY) > b.w ? 2 : 1;
  const int foot_h = has_note ? note_lines * body_h : bar ? bar_h : 0;
  const int foot = foot_h ? gap + foot_h : 0;
  const int side_w = side.empty() ? 0 : ui::text_width(side, Font::BODY);

  // The largest font that fits the block with what lies under it.
  static const Font VALUES[] = {Font::DIGITS, Font::VALUE, Font::HEADLINE, Font::TITLE, Font::BODY_LARGE, Font::BODY};
  static const Font ICONS[] = {Font::ICON, Font::ICON_SMALL};
  const Font *fonts = icon ? ICONS : VALUES;
  const int nfonts = icon ? 2 : 6;
  Font font = fonts[nfonts - 1];
  bool fits = false, name_under = false;
  for (int i = 0; i < nfonts; ++i) {
    const Font f = fonts[i];
    if (f == Font::DIGITS && !ui::covers(main, f)) continue;
    const int mh = ui::line_height(f);
    if (icon) {
      if (mh + foot > b.h) continue;
      font = f;
      fits = true;
      name_under = side_w > 0 && mh + gap + body_h + foot <= b.h;
      break;
    }
    const int w = ui::text_width(main, f) + (side_w ? gap + side_w : 0);
    if (w <= b.w && mh + foot <= b.h) {
      font = f;
      fits = true;
      break;
    }
  }
  // No room for both: the note stays, the value goes.
  const bool show_main = fits || !has_note;
  const int mh = show_main ? ui::line_height(font) : 0;
  const int total = mh + (name_under ? gap + body_h : 0) + (show_main ? foot : foot_h);
  const int y0 = b.y + (b.h - total) / 2;

  ui::set_font(value_, font);
  ui::set_text(value_, main);
  const Tone tone = moving && usable && !icon ? Tone::ACCENT : Tone::NORMAL;
  const theme::Role ink = usable ? theme::INK : theme::MUTED;
  ui::set_tone(value_, tone, ink);
  show(value_, show_main);
  if (icon) {
    lv_obj_set_width(value_, b.w);
    set_align(value_, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_pos(value_, b.x, y0);
    lv_obj_set_width(unit_label_, b.w);
    set_align(unit_label_, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_pos(unit_label_, b.x, y0 + mh + gap);
    show(unit_label_, show_main && name_under);
  } else {
    const int tw = ui::text_width(main, font);
    const int x0 = b.x + (b.w - tw - (side_w ? gap + side_w : 0)) / 2;
    lv_obj_set_width(value_, LV_SIZE_CONTENT);
    set_align(value_, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_pos(value_, x0, y0);
    // The unit on the value's baseline.
    const lv_font_t *vf = ui::font(font), *uf = ui::font(Font::BODY);
    const int base = vf && uf ? (mh - vf->base_line) - (body_h - uf->base_line) : mh - body_h;
    lv_obj_set_width(unit_label_, LV_SIZE_CONTENT);
    set_align(unit_label_, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_pos(unit_label_, x0 + tw + gap, y0 + base);
    show(unit_label_, show_main && side_w > 0);
  }
  ui::set_text(unit_label_, side);
  ui::set_tone(unit_label_, tone, ink);

  const int fy = y0 + mh + (name_under ? gap + body_h : 0) + (show_main ? gap : 0);
  lv_obj_set_pos(note_, b.x, fy);
  lv_obj_set_size(note_, b.w, note_lines * body_h);
  ui::set_text(note_, note ? note : "");
  ui::set_color(note_, theme::MUTED);
  show(note_, has_note);

  lv_obj_set_pos(track_, b.x, fy);
  lv_obj_set_size(track_, b.w, bar_h);
  ui::set_fill(track_, Tone::NORMAL, theme::TRACK);
  const int fw = b.w * position_ / 100;
  lv_obj_set_pos(fill_, 0, 0);
  lv_obj_set_size(fill_, fw, bar_h);
  ui::set_fill(fill_, Tone::NORMAL, usable ? theme::ACCENT : theme::MUTED);
  show(track_, bar && show_main);
  show(fill_, fw > 0);
}

void DeskTile::paint_key(int slot, Look look) {
  theme::Role ground = theme::TRACK, ink = theme::INK;
  Tone tone = Tone::NORMAL;
  switch (look) {
    case Look::NORMAL: break;
    case Look::ACCENT:
      ground = theme::ACCENT;
      ink = theme::ON_ACCENT;
      break;
    // The busy tone is Home Assistant's amber, which no glyph reads in on a light ground: the key is amber instead,
    // pale in light and deep in dark, under INK.
    case Look::BUSY: ground = theme::AMBER_TRACK; break;
    case Look::PRESSED: ground = theme::RAISED; break;
    case Look::ALERT: tone = Tone::ALERT; break;
    case Look::MUTED: ink = theme::SUBTLE; break;
  }
  ui::set_fill(key_[slot], Tone::NORMAL, ground);
  // A mask takes the colour the icon label of that look takes.
  if (key_masked_[slot]) set_style_color(key_main_[slot], LV_STYLE_IMAGE_RECOLOR, ui::tone_color(tone, ink));
  else if (key_main_[slot]) ui::set_tone(key_main_[slot], tone, ink);
  if (key_word_[slot]) ui::set_tone(key_word_[slot], tone, ink);
}

}  // namespace esphome::loctek_desk
