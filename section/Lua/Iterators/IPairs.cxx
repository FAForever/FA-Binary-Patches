#include "IPairs.h"

IPairsIterator::IPairsIterator(const LuaObject &table)
    : table{table}, index{0}, value{table.m_state} {}

IPairsIterator &IPairsIterator::operator++() {
  ++index;
  value = table[index];
  return *this;
}

const std::pair<int, LuaObject> IPairsIterator::operator*() const {
  return {index, value};
}

std::pair<int, LuaObject> IPairsIterator::operator*() { return {index, value}; }

bool IPairsIterator::operator!=(const IPairsEndIterator &) const {
  return !value.IsNil();
}

IPairs::IPairs(const LuaObject &table) : table{table} {
  luaplus_assert(table.IsTable());
}

IPairsIterator IPairs::begin() { return ++IPairsIterator(table); }
IPairsEndIterator IPairs::end() { return IPairsEndIterator{}; }
