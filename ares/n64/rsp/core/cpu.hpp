//{
  union r32 {
    struct {  int32_t i32; };
    struct { uint32_t u32; };
  };

  struct Core {
    enum Register : uint {
      R0, AT, V0, V1, A0, A1, A2, A3,
      T0, T1, T2, T3, T4, T5, T6, T7,
      S0, S1, S2, S3, S4, S5, S6, S7,
      T8, T9, K0, K1, GP, SP, S8, RA,
    };

    r32 r[32];
    r32 lo;
    r32 hi;
    u32 pc;

    //internal
    maybe<u32> ip;
  } core;

  //cpu-instructions.cpp
  auto instructionADDIU() -> void;
  auto instructionADDU() -> void;
  auto instructionAND() -> void;
  auto instructionANDI() -> void;
  auto instructionB(bool) -> void;
  auto instructionBREAK() -> void;
  auto instructionCACHE() -> void;
  auto instructionJ() -> void;
  auto instructionJAL() -> void;
  auto instructionJALR() -> void;
  auto instructionJR() -> void;
  auto instructionLB() -> void;
  auto instructionLBU() -> void;
  auto instructionLH() -> void;
  auto instructionLHU() -> void;
  auto instructionLUI() -> void;
  auto instructionLW() -> void;
  auto instructionNOR() -> void;
  auto instructionOR() -> void;
  auto instructionORI() -> void;
  auto instructionSB() -> void;
  auto instructionSH() -> void;
  auto instructionSLL() -> void;
  auto instructionSLLV() -> void;
  auto instructionSLTIU() -> void;
  auto instructionSLTU() -> void;
  auto instructionSRA() -> void;
  auto instructionSRAV() -> void;
  auto instructionSRL() -> void;
  auto instructionSRLV() -> void;
  auto instructionSUBU() -> void;
  auto instructionSW() -> void;
  auto instructionXOR() -> void;
  auto instructionXORI() -> void;
//};