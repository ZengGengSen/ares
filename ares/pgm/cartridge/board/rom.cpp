struct Rom : Interface {
  using Interface::Interface;

  // Pxxxx: 68000 program
  Memory::Readable<n16> prom;
  // Txxxx: Text and background graphics
  // (2 formats within the same rom)
  Memory::Readable<n8 > trom;
  // Mxxxx: Audio samples (8 bit mono signed PCM at 11025Hz)
  Memory::Readable<n8 > mrom;
// Axxxx: Sprite graphics data
  // Bxxxx: A Offsets and Sprite masks
  Memory::Readable<n8 > sram;

  auto load() -> void override {
    Interface::load(prom, "program.rom");
    Interface::load(trom, "text.rom");
    Interface::load(mrom, "music.rom");
    Interface::load(sram, "sprite.rom");
  }

  auto unload() -> void override {
    prom.reset();
    trom.reset();
    mrom.reset();
    sram.reset();
  }

  auto readP(n1 upper, n1 lower, n24 address, n16 data) -> n16 override {
    if(address <= 0x4fffff) return prom[address >> 1];
    return data;
  }

  auto readT(n32 address) -> n8 override {
    return trom[address];
  }

  auto readM(n32 address) -> n8 override {
    return mrom[address];
  }

  auto readS(n32 address) -> n8 override {
    return sram[address];
  }
};
