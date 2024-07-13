#include <pgm/pgm.hpp>

namespace ares::PolyGameMaster {

Cartridge& cartridge = cartridgeSlot.cartridge;
#include "board/board.cpp"
#include "serialization.cpp"
#include "slot.cpp"

auto Cartridge::allocate(Node::Port parent) -> Node::Peripheral {
  return node = parent->append<Node::Peripheral>("PolyGameMaster Cartridge");
}

auto Cartridge::connect() -> void {
  if(!node->setPak(pak = platform->pak(node))) return;

  information = {};
  information.title = pak->attribute("title");
  information.board = pak->attribute("board");

  board = new Board::Rom{*this};
  board->pak = pak;
  board->load();

  power();
}

auto Cartridge::disconnect() -> void {
  if (!node || !board) return;
  board->unload();
  board->pak.reset();
  board.reset();
  node.reset();
}

auto Cartridge::save() -> void {
  if(!node) return;
  if(board) board->save();
}

auto Cartridge::power() -> void {
  if(board) board->power();
}

auto Cartridge::readP(n1 upper, n1 lower, n24 address, n16 data) -> n16 {
  if(board) return board->readP(upper, lower, address, data);
  return data;
}

auto Cartridge::readAP(n1 upper, n1 lower, n24 address, n16 data) -> n16 {}

}
