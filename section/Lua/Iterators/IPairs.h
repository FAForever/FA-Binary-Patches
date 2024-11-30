#pragma once
#include "LuaAPI.h"
#include <utility>

class IPairsEndIterator {
};

class IPairsIterator {
public:
  IPairsIterator(const LuaObject &table);

  IPairsIterator &operator++();

  const std::pair<int, LuaObject> operator*() const;

  std::pair<int, LuaObject> operator*();

  bool operator!=(const IPairsEndIterator &) const;

private:
  const LuaObject &table;
  int index;
  LuaObject value;
};

class IPairs {
public:
  IPairs(const LuaObject &table);

  IPairsIterator begin();
  IPairsEndIterator end();

private:
  const LuaObject &table;
};