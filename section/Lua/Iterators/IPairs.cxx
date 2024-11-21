#include "IPairs.h"

IPairsEndIterator::IPairsEndIterator(const LuaObject &table)
    : n{table.GetN()} {}

int IPairsEndIterator::GetN() const { return n; }

IPairsIterator::IPairsIterator(const LuaObject &table)
    : table{table}, index{0}, value{table.m_state} {}

IPairsIterator &IPairsIterator::operator++() {
  LuaState *state = table.m_state;
  ++index;
  value = table[index];
  return *this;
}

const std::pair<int, LuaObject> IPairsIterator::operator*() const {
  return {index, value};
}

std::pair<int, LuaObject> IPairsIterator::operator*() { return {index, value}; }

bool IPairsIterator::operator!=(const IPairsEndIterator &end_it) const {
  return index <= end_it.GetN();
}

IPairs::IPairs(const LuaObject &table) : table{table} {
  luaplus_assert(table.IsTable());
}

IPairsIterator IPairs::begin() { return ++IPairsIterator(table); }
IPairsEndIterator IPairs::end() { return IPairsEndIterator{table}; }
