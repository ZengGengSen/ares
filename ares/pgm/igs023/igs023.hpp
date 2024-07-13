// L8A0290 IGS023(QFP256)

struct IGS023 : Thread {
  Node::Object node;
  Node::Video::Screen screen;

  Memory::Writable<n16> bgram;
  Memory::Writable<n16> txtram;
  Memory::Writable<n16> rowram;

  Memory::Writable<n16> palram;
  
  // igs023.cpp
  auto load(Node::Object parent) -> void;
  auto unload() -> void;

  auto step(u32 clocks) -> void;
  auto main() -> void;
  auto frame() -> void;
  auto power(bool reset) -> void;

  // memory.cpp
  auto read(n14 address) -> n16;
  auto write(n14 address, n16 data) -> void;

  auto readPalette(n12 address) -> n16;
  auto writePalette(n12 address, n16 data) -> void;

  auto readIO(n1 upper, n1 lower, n24 address, n16 data) -> n16;
  auto writeIO(n1 upper, n1 lower, n24 address, n16 data) -> void;

  //render.cpp
  auto render() -> void;

  //color.cpp
  auto color(n32) -> n64;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  struct IO {
    n9  hcounter;
    n9  vcounter;
  } io;
};

extern IGS023 igs023;
