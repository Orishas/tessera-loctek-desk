#include "desk_tile.h"

namespace esphome::loctek_desk {

using tessera::Font;
namespace ui = tessera::ui;

void DeskTile::create(const tessera::TileContext &c) {
  ui::set_text(ui::label(c.parent, Font::BODY, theme::MUTED), plugin_->text("choose_desk"));
}

}  // namespace esphome::loctek_desk
