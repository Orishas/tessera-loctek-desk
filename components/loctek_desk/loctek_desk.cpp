#include "loctek_desk.h"
#include "desk_tile.h"

namespace esphome::loctek_desk {

void LoctekDesk::setup() {
  // The id is the tile's id in the manifest; the core makes a DeskTile for every card that shows one.
  add_tile("desk", [this]() { return new DeskTile(this); });
}

}  // namespace esphome::loctek_desk
