auto CPU::read(n1 upper, n1 lower, n24 address, n16 data) -> n16 {
  // bios rom
  if (address < 0x100000)
    return system.bios[address >> 1];

  // cartridge program rom
  if (address < 0x600000)
    return cartridge.readP(upper, lower, address, data);

  // need 0x700006,0x700007 watchdog?
  if (address < 0x800000)
    return data;

  // work ram
  if (address < 0x900000) {
    address &= 0x01ffff;
    return system.wram[address >> 1];
  }

  if (address < 0xa00000)
    return igs023.read(address >> 1);

  if (address < 0xb00000)
    return igs023.readPalette(address >> 1);

  if (address < 0xc00000)
    return igs023.readIO(upper, lower, address, data);

  // io registers
  if (address < 0xc10000)
    return readIO(upper, lower, address, data);

  // apu program ram
  if (address < 0xd00000) {
    address &= 0x00ffff;
    return cartridge.readAP(upper, lower, address, data);
  }

  return data;
}

auto CPU::write(n1 upper, n1 lower, n24 address, n16 data) -> void {
  if (address < 0x800000) return;

  if (address < 0x900000) {
    address &= 0x01ffff;
    system.wram[address >> 1] = data;
    return;
  }

  if (address < 0xa00000)
    return igs023.write(address >> 1, data);

  if (address < 0xb00000)
    return igs023.writePalette(address >> 1, data);

  if (address < 0xc00000)
    return igs023.writeIO(upper, lower, address, data);

  if (address < 0xc10000) {
    writeIO(upper, lower, address, data);
    return;
  }
}

auto CPU::readIO(n1 upper, n1 lower, n24 address, n16 data) -> n16 {
  address &= ~0x800e;

  if (address == 0x0004) {
    // todo: sync z80
    // todo: return ics2115
  }
  
  if (address == 0x0006) {
    // todo: return ics2115
  }

  if (address == 0x8000) {
    // todo: return 1p + 2p inputs
  }

  if (address == 0x8002) {
    // todo: return 3p + 4p inputs
  }

  if (address == 0x8004) {
    // todo: return extra inputs
  }

  if (address == 0x8006) {
    // todo: return dip switches
    return 0xff00;
  }

  return data;
}

auto CPU::writeIO(n1 upper, n1 lower, n24 address, n16 data) -> void {
  address &= ~0x800e;

  if (address == 0x0002) {
    // todo: sync z80
    // todo: write ics2115 0
    // todo: trigger z80 nmi
  }

  if (address == 0x0004) {
    // todo: sync z80
    // todo: write ics2115 1
  }

  if (address == 0x0006) {
    // todo: write v3021
  }

  if (address == 0x0008) {
    // todo: sync z80
    if (data == 0x5050) {
      // todo: reset ics2115
      // todo: z80 halt & reset
    } else {
      // todo: z80 not halt
    }
  }

  if (address == 0x000a) {
    // todo: connect z80 controller
  }

  if (address == 0x000c) {
    // todo: sync z80
    // todo: write ics2115 2
  }
}
