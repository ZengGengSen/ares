auto PPU::color(n32 color) -> n64 {
  n32 r = color.bit( 0, 4);
  n32 g = color.bit( 5, 9);
  n32 b = color.bit(10,14);
  n32 l = color.bit(15,18);

  //luma=0 is not 100% black; but it's much darker than normal linear scaling.
  //exact effect seems to be analog; requires > 24-bit color depth to represent accurately.
  f64 L = (1.0 + l) / 16.0 * (l ? 1.0 : 0.25);
  n64 R = L * image::normalize(r, 5, 16);
  n64 G = L * image::normalize(g, 5, 16);
  n64 B = L * image::normalize(b, 5, 16);

  if(deepBlackBoost->value()) {
    static const n8 gammaRamp[32] = {
      0x00, 0x01, 0x03, 0x06, 0x0a, 0x0f, 0x15, 0x1c,
      0x24, 0x2d, 0x37, 0x42, 0x4e, 0x5b, 0x69, 0x78,
      0x88, 0x90, 0x98, 0xa0, 0xa8, 0xb0, 0xb8, 0xc0,
      0xc8, 0xd0, 0xd8, 0xe0, 0xe8, 0xf0, 0xf8, 0xff,
    };
    R = L * gammaRamp[r] * 0x0101;
    G = L * gammaRamp[g] * 0x0101;
    B = L * gammaRamp[b] * 0x0101;
  }

  return R << 32 | G << 16 | B << 0;
}
