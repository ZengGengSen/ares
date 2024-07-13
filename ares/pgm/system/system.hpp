struct System {
  Node::System node;
  VFS::Pak pak;

  Memory::Readable<n16> bios;
  Memory::Writable<n16> wram;

  enum class Model : u32 {
    PolyGameMaster1,
    // PolyGameMaster2,
    // PolyGameMaster3,
  };

  auto name() const -> string { return information.name; }
  auto frequency() const -> double { return 20'000'000.0; }
  auto model() const -> Model { return information.model; }

  //system.cpp
  auto game() -> string;
  auto run() -> void;

  auto load(Node::System& node, string name) -> bool;
  auto unload() -> void;
  auto save() -> void;
  auto power(bool reset = false) -> void;

  //serialization.cpp
  auto serialize(bool synchronize) -> serializer;
  auto unserialize(serializer&) -> bool;

private:
  struct Information {
    string name;
    Model model = Model::PolyGameMaster1;
  } information;
  
  //serialization.cpp
  auto serialize(serializer&, bool synchronize) -> void;
};

extern System system;
