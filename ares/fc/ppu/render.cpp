auto PPU::enable() const -> bool {
  return io.bgEnable || io.spriteEnable;
}

auto PPU::rendering() const -> bool {
  return enable() && (io.ly < 240 || io.ly == vlines() - 1);
}

auto PPU::loadCHR(n16 address) -> n8 {
  if(enable()) {
    io.busAddress = (n14)address;
    cartridge.ppuAddressBus(address);
    return cartridge.readCHR(address);
  } else {
    return 0x00;
  }
}

auto PPU::renderPixel() -> void {
  if(io.ly >= screen->canvasHeight()) return;

  u32  x              = io.lx - 1;
  u32  mask           = 0x8000 >> (scroll.fineX + (x & 7));
  u32  palette        = 0;
  u32  objectPalette  = 0;
  bool objectPriority = 0;

  //PAL systems technically blank the topmost scanline and a 2px column on each side of active display.
  if(Region::PAL())
    if(io.ly == 0 || x < 2 || x > 253) return;

  palette |= latch.tiledataLo & mask ? 1 : 0;
  palette |= latch.tiledataHi & mask ? 2 : 0;
  if(palette) {
    u32 attr = latch.attribute;
    if(mask >= 256) attr >>= 2;
    palette |= (attr & 3) << 2;
  }

  if(!io.bgEnable) palette = 0;
  if(!io.bgEdgeEnable && x < 8) palette = 0;

  if(io.spriteEnable)
    for(i32 sprite = 7; sprite >= 0; sprite--) {
      if(!io.spriteEdgeEnable && x < 8) continue;
      if(latch.oam[sprite].id == 64) continue;

      u32 spriteX = x - latch.oam[sprite].x;
      if(spriteX >= 8) continue;

      if(latch.oam[sprite].attr & 0x40) spriteX ^= 7;
      u32 mask          = 0x80 >> spriteX;
      u32 spritePalette = 0;
      spritePalette |= latch.oam[sprite].tiledataLo & mask ? 1 : 0;
      spritePalette |= latch.oam[sprite].tiledataHi & mask ? 2 : 0;
      if(spritePalette == 0) continue;

      if(latch.oam[sprite].id == 0 && palette && x != 255) io.spriteZeroHit = 1;
      spritePalette |= (latch.oam[sprite].attr & 3) << 2;

      objectPriority = latch.oam[sprite].attr & 0x20;
      objectPalette  = 16 + spritePalette;
    }

  if(objectPalette) {
    if(palette == 0 || objectPriority == 0) palette = objectPalette;
  }

  u32 color = 0;
  if(enable() || (n14)var.address < 0x3f00) {
    color = io.emphasis << 6 | readCGRAM(palette);
  } else {
    color = io.emphasis << 6 | readCGRAM((n5)var.address);
  }

  if(Region::PAL())
    output[(x + 18) % 283] = color;
  else
    output[(x + 16) % 283] = color;
}

template<u32 Cycles>
inline auto PPU::cyclePictureAddress() -> void {
  constexpr u32 Id = (Cycles - 1) & 7;

  if constexpr(Id == 0) io.pictureAddress = 0x2000 | (n12)var.address;
  if constexpr(Id == 2) io.pictureAddress = 0x23c0 | var.nametable << 10 | (var.tileY >> 2) << 3 | var.tileX >> 2;
  if constexpr(Id == 4) {
    // TODO: Use the oamData buffer to control picture address
    constexpr bool IsSprite = Cycles >= 257 && Cycles <= 320;
    constexpr u32  SpriteId = (Cycles - 257) >> 3;
    if constexpr(IsSprite) {
      if(io.spriteHeight == 16) {
        n4 spriteY = io.ly - latch.oam[SpriteId].y;
        if(latch.oam[SpriteId].attr & 0x80) spriteY ^= 15;
        io.pictureAddress = (latch.oam[SpriteId].tile & 1) << 12 | (latch.oam[SpriteId].tile & ~1) << 4 |
                            spriteY.bit(3) << 4 | n3(spriteY);
      } else {
        n3 spriteY = io.ly - latch.oam[SpriteId].y;
        if(latch.oam[SpriteId].attr & 0x80) spriteY ^= 7;
        io.pictureAddress = io.spriteAddress | latch.oam[SpriteId].tile << 4 | spriteY;
      }
    } else {
      io.pictureAddress = io.bgAddress | io.picAddrNametable << 4 | var.fineY;
    }
  }
  if constexpr(Id == 6) io.pictureAddress |= 8;
}

template<u32 Cycles>
inline auto PPU::cycleDataFetched() -> void {
  constexpr u32 Id = (Cycles - 1) & 7;

  if constexpr(Id == 0) io.picAddrNametable = loadCHR(io.pictureAddress);
  if constexpr(Id == 2) {
    io.picAddrAttribute = loadCHR(io.pictureAddress);
    if(var.tileY & 2) io.picAddrAttribute >>= 4;
    if(var.tileX & 2) io.picAddrAttribute >>= 2;
  }
  if constexpr(Id == 4) io.picAddrTiledataLo = loadCHR(io.pictureAddress);
  if constexpr(Id == 6) io.picAddrTiledataHi = loadCHR(io.pictureAddress);
}

auto PPU::shiftRegister() -> void {
  latch.nametable  = latch.nametable << 8 | io.picAddrNametable;
  latch.attribute  = latch.attribute << 2 | (io.picAddrAttribute & 3);
  latch.tiledataLo = latch.tiledataLo << 8 | io.picAddrTiledataLo;
  latch.tiledataHi = latch.tiledataHi << 8 | io.picAddrTiledataHi;
}

template<u32 Cycles>
auto PPU::assignSpriteTileData() -> void {
  constexpr u32 SpriteId         = (Cycles - 257) >> 3;
  latch.oam[SpriteId].tiledataLo = io.picAddrTiledataLo;
  latch.oam[SpriteId].tiledataHi = io.picAddrTiledataHi;
}

template<u32 Cycles>
inline auto PPU::cycle() -> void {
  if constexpr(Cycles >= 1 && Cycles <= 339 && ((Cycles & 1) == 1)) cyclePictureAddress<Cycles>();
  if constexpr(Cycles >= 1 && Cycles <= 339 && ((Cycles & 1) == 1)) cycleDataFetched<Cycles>();
  if constexpr(Cycles >= 1 && Cycles <= 256) renderPixel();
  if constexpr(((Cycles >= 1 && Cycles <= 256) || (Cycles >= 321 && Cycles <= 339)) && ((Cycles - 1) & 7) == 7)
    shiftRegister();
  if constexpr(Cycles >= 257 && Cycles <= 320 && ((Cycles - 1) & 7) == 7) assignSpriteTileData<Cycles>();

  step(1);
}

auto PPU::renderScanline() -> void {
  if(io.ly < screen->canvasHeight()) {
    output = screen->pixels().data() + io.ly * 283;
    for(auto n : range(283)) output[n] = Region::PAL() ? 0x3f : io.emphasis << 6 | readCGRAM(0);
  }

  //Vblank
  if(io.ly >= 240 && io.ly <= vlines() - 2) return step(341), scanline();

  //  0
  cycle<0>();

  // force clear sprite counter at start of each scanline
  for(auto& id : latch.oamId) id = 64;

  // clang-format off
  #define cycle02(Cycles) cycle<Cycles>(); cycle<Cycles +  1>()
  #define cycle04(Cycles) cycle02(Cycles); cycle02(Cycles +  2)
  #define cycle08(Cycles) cycle04(Cycles); cycle04(Cycles +  4)
  #define cycle16(Cycles) cycle08(Cycles); cycle08(Cycles +  8)
  #define cycle32(Cycles) cycle16(Cycles); cycle16(Cycles + 16)
  #define cycle64(Cycles) cycle32(Cycles); cycle32(Cycles + 32)

  //  1-256
  cycle64(  1);
  cycle64( 65);
  cycle64(129);
  cycle64(193);
  // clang-format on

  for(u32 n : range(8)) {
    latch.oam[n].id   = latch.oamId[n];
    latch.oam[n].y    = soam[4 * n + 0];
    latch.oam[n].tile = soam[4 * n + 1];
    latch.oam[n].attr = soam[4 * n + 2];
    latch.oam[n].x    = soam[4 * n + 3];
  }

  cycle64(257);  //257-320
  cycle16(321);  //321-336

  bool skip = !Region::PAL() && enable() && io.field == 1 && io.ly == vlines() - 1;
  cycle02(337);  //337-338
  cycle<339>();  //339

  //340
  if(!skip) cycle<340>();

  return scanline();
}
