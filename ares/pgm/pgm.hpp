#pragma once
// started: 2024-07-13
#include <ares/ares.hpp>

#include <component/processor/arm7tdmi/arm7tdmi.hpp>
#include <component/processor/m68000/m68000.hpp>
#include <component/processor/z80/z80.hpp>

namespace ares::PolyGameMaster {
  #include <ares/inline.hpp>

  auto enumerate() -> vector<string>;
  auto load(Node::System& node, string name) -> bool;

  struct Model {
    inline static auto PolyGameMaster1() -> bool;
    inline static auto PolyGameMaster2() -> bool;
    inline static auto PolyGameMaster3() -> bool;
  };

  #include <pgm/cartridge/cartridge.hpp>
  #include <pgm/cpu/cpu.hpp>
  #include <pgm/igs023/igs023.hpp>
  #include <pgm/system/system.hpp>
}
