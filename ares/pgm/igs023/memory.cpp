auto IGS023::read(n14 address) -> n16 {
  n6 offset = address;

  if      (address < 0x2000) return bgram [offset];
  else if (address < 0x3800) return txtram[offset];
  else                       return rowram[offset];
}

auto IGS023::write(n14 address, n16 data) -> void {
  n6 offset = address >> 1;

  if      (address < 0x2000) bgram [offset] = data;
  else if (address < 0x3800) txtram[offset] = data;
  else                       rowram[offset] = data;
}

auto IGS023::readPalette(n12 address) -> n16 {
  return palram[address];
}

auto IGS023::writePalette(n12 address, n16 data) -> void {
  palram[address] = data;
}

auto IGS023::readIO(n1 upper, n1 lower, n24 address, n16 data) -> n16 {
  // 0xb01000-0xb0103f: Zoom/Shrink table for sprites
  // 0xb02000-0xb02001: Background scroll Y
  // 0xb03000-0xb03001: Background scroll X
  // 0xb04000-0xb04001: Unknown #0
  // 0xb05000-0xb05001: Foreground scroll Y
  // 0xb06000-0xb06001: Foreground scroll X?
  // 0xb0e000-0xb0e001: Unknown #1

  return data;
}

auto IGS023::writeIO(n1 upper, n1 lower, n24 address, n16 data) -> void {
  // 0xb01000-0xb0103f: Zoom/Shrink table for sprites
  // 0xb02000-0xb02001: Background scroll Y
  // 0xb03000-0xb03001: Background scroll X
  // 0xb04000-0xb04001: Unknown #0
  // 0xb05000-0xb05001: Foreground scroll Y
  // 0xb06000-0xb06001: Foreground scroll X?
  // 0xb0e000-0xb0e001: Unknown #1
}
