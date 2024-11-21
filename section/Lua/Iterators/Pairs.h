#pragma once
#include "LuaAPI.h"
#include <utility>

class EndIterator {};
class PairsIterator {
public:
  PairsIterator(const LuaObject &table);

  PairsIterator &operator++();

  const std::pair<LuaObject, LuaObject> operator*() const;

  std::pair<LuaObject, LuaObject> operator*();

  bool operator!=(const EndIterator &) const;

private:
  const LuaObject &table;
  LuaObject key;
  LuaObject value;
  bool done;
};

class Pairs {
public:
  Pairs(const LuaObject &table);
  PairsIterator begin();
  EndIterator end();

private:
  const LuaObject &table;
};