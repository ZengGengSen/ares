struct CPU : M68000, Thread {
  Node::Object node;

  enum class Interrupt : u32 {
    Reset,
    Timer,
    Vblank,
  };

  // cpu.cpp
  auto load(Node::Object) -> void;
  auto unload() -> void;

  auto main() -> void;
  auto idle(u32 clocks) -> void override;
  auto wait(u32 clocks) -> void override;

  auto raise(Interrupt) -> void;
  auto lower(Interrupt) -> bool;

  auto power(bool reset) -> void;

  // memory.cpp
  auto read(n1 upper, n1 lower, n24 address, n16 data = 0) -> n16 override;
  auto write(n1 upper, n1 lower, n24 address, n16 data) -> void override;

  auto readIO(n1 upper, n1 lower, n24 address, n16 data) -> n16;
  auto writeIO(n1 upper, n1 lower, n24 address, n16 data) -> void;

  // serialization.cpp
  auto serialize(serializer&) -> void;

  struct IO {
    n32 interruptPending;
  } io;
};

extern CPU cpu;
