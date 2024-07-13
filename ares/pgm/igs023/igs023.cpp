#include <pgm/pgm.hpp>

namespace ares::PolyGameMaster {

IGS023 igs023;
#include "memory.cpp"

auto IGS023::load(Node::Object parent) -> void {
  node = parent->append<Node::Object>("IGS023");

  screen = node->append<Node::Video::Screen>("Screen", 448, 224);
  screen->colors(1 << 15, {&IGS023::color, this});
  screen->setSize(448, 224);
  screen->setScale(1.0, 1.0);
  screen->setAspect(1.0, 1.0);
  screen->refreshRateHint(system.frequency() / 2, 448, 224);

  bgram.allocate(16_KiB >> 1);
  txtram.allocate(8_KiB >> 1);
  rowram.allocate(2_KiB >> 1);
  palram.allocate(4608 >> 1);
}

auto IGS023::unload() -> void {
  bgram.reset();
  txtram.reset();
  rowram.reset();
  palram.reset();

  screen->quit();
  node->remove(screen);
  screen.reset();
  node.reset();
}

auto IGS023::step(u32 clocks) -> void {
  Thread::step(clocks);
  Thread::synchronize();
}

auto IGS023::main() -> void {
  step(1);

  if (++io.hcounter == 640) {
    io.hcounter = 0;
    ++io.vcounter;

    if (io.vcounter == 218) cpu.raise(CPU::Interrupt::Timer);
    if (io.vcounter == 224) cpu.raise(CPU::Interrupt::Vblank);

    if (io.vcounter == 264) {
      io.vcounter = 0;
      frame();
    }
  }

  if (io.vcounter >= 0 && io.vcounter <= 223 && io.hcounter == 0) {
    render();
  }
}

auto IGS023::frame() -> void {
  screen->setViewport(0, 0, 448, 224);
  screen->frame();
  scheduler.exit(Event::Frame);
}

auto IGS023::power(bool reset) -> void {
  // or 50MHz / 5?
  Thread::create(system.frequency() / 2, { &IGS023::main, this });
  screen->power();
  
  if (!reset) {
    bgram.fill();
    txtram.fill();
    rowram.fill();
    palram.fill();
  }
}

}
