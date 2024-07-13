#include <pgm/pgm.hpp>

namespace ares::PolyGameMaster {

auto enumerate() -> vector<string> {
  return {
    "[IGS] PolyGame Master 1",
    // "[IGS] PolyGame Master 2",
    // "[IGS] PolyGame Master 3",
  };
}

auto load(Node::System& node, string name) -> bool {
  if(!enumerate().find(name)) return false;
  return system.load(node, name);
}

Scheduler scheduler;
System system;
#include "serialization.cpp"

auto System::game() -> string {
  if (cartridge.node) {
    return cartridge.title();
  }

  return "(no cartridge connected)";
}

auto System::run() -> void {
  scheduler.enter();
}

auto System::load(Node::System& root, string name) -> bool {
  if(node) unload();

  information = {};
  if(name.find("PolyGame Master 1")) {
    information.name = "PolyGame Master 1";
    information.model = Model::PolyGameMaster1;
  }
#if 0
  if(name.find("PolyGame Master 2")) {
    information.name = "PolyGame Master 2";
    information.model = Model::PolyGameMaster2;
  }
  if(name.find("PolyGame Master 3")) {
    information.name = "PolyGame Master 3";
    information.model = Model::PolyGameMaster3;
  }
#endif

  node = Node::System::create(information.name);
  node->setGame({&System::game, this});
  node->setRun({&System::run, this});
  node->setPower({&System::power, this});
  node->setSave({&System::save, this});
  node->setUnload({&System::unload, this});
  node->setSerialize({&System::serialize, this});
  node->setUnserialize({&System::unserialize, this});
  root = node;
  if(!node->setPak(pak = platform->pak(node))) return false;

  wram.allocate(128_KiB >> 1);

  scheduler.reset();
  cpu.load(node);
  cartridgeSlot.load(node);

  // TODO: Component load

  return true;
}

auto System::unload() -> void {
  if (!node) return;
  
  save();
  // TODO: Component unload
  cpu.unload();
  cartridgeSlot.unload();
  wram.reset();
  pak.reset();
  node.reset();
}

auto System::save() -> void {
  if (!node) return;

  cartridge.save();
  // TODO: Save memory
}

auto System::power(bool reset) -> void {
  for (auto& setting : node->find<Node::Setting::Setting>()) {
  }
}

} // namespace ares::PolyGameMaster
