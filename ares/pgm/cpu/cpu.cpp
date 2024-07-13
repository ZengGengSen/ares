#include <pgm/pgm.hpp>

namespace ares::PolyGameMaster {

CPU cpu;
#include "memory.cpp"
#include "serialization.cpp"

auto CPU::load(Node::Object parent) -> void {
  node = parent->append<Node::Object>("CPU");
  // debugger.load(node);
}

auto CPU::unload() -> void {
  // debugger = {};
  node.reset();
}

auto CPU::main() -> void {
  if(io.interruptPending) {
    if(lower(Interrupt::Reset)) {
      r.a[7] = read(1, 1, 0) << 16 | read(1, 1, 2) << 0;
      r.pc   = read(1, 1, 4) << 16 | read(1, 1, 6) << 0;
      prefetch();
      prefetch();
      // debugger.interrupt("Reset");
    }

    if (6 > r.i && lower(Interrupt::Vblank)) {
      // debugger.interrupt("Vblank");
      return interrupt(Vector::Level6, 6);
    }

    if (4 > r.i && lower(Interrupt::Timer)) {
      // debugger.interrupt("Timer");
      return interrupt(Vector::Level4, 4);
    }
  }

  // debugger.instruction();
  instruction();
}

auto CPU::idle(u32 clocks) -> void {
  Thread::step(clocks);
}

auto CPU::wait(u32 clocks) -> void {
  Thread::step(clocks);
  Thread::synchronize();
}

auto CPU::raise(Interrupt interrupt) -> void {
  io.interruptPending.bit((u32)interrupt) = 1;
}

auto CPU::lower(Interrupt interrupt) -> bool {
  bool pending = io.interruptPending.bit((u32)interrupt);
  io.interruptPending.bit((u32)interrupt) = 0;
  return pending;
}

auto CPU::power(bool reset) -> void {
  M68000::power();
  Thread::create(system.frequency(), { &CPU::main, this });
  io.interruptPending = 0;
  raise(Interrupt::Reset);
}

}  // namespace ares::PolyGameMaster
