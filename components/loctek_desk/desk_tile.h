#pragma once
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::loctek_desk {

// The desk tile. For now it only asks for a desk to be chosen.
class DeskTile : public tessera::Tile {
 public:
  explicit DeskTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override;

 private:
  const tessera::Plugin *plugin_;
};

}  // namespace esphome::loctek_desk
