#pragma once
#include "LuaAPI.h"
#include <utility>

class IPairsEndIterator {
public:
  IPairsEndIterator(const LuaObject &table);

  int GetN() const;

private:
  int n;
};

class IPairsIterator {
public:
  IPairsIterator(const LuaObject &table);

  IPairsIterator &operator++();

  const std::pair<int, LuaObject> operator*() const;

  std::pair<int, LuaObject> operator*();

  bool operator!=(const IPairsEndIterator &end_it) const;

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