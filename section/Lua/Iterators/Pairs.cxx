#include "Pairs.h"

PairsIterator::PairsIterator(const LuaObject &table)
    : table{table}, key{table.m_state}, value{table.m_state}, done{false} {}

PairsIterator &PairsIterator::operator++()
{
    LuaState *state = table.m_state;
    if (!done && !LuaPlusH_next(state, &table, &key, &value))
    {
        done = true;
    }
    return *this;
}

const std::pair<LuaObject, LuaObject> PairsIterator::operator*() const
{
    return {key, value};
}

std::pair<LuaObject, LuaObject> PairsIterator::operator*()
{
    return {key, value};
}

bool PairsIterator::operator!=(const EndIterator &) const { return !done; }

Pairs::Pairs(const LuaObject &table) : table{table}
{
    luaplus_assert(table.IsTable());
}

PairsIterator Pairs::begin() { return ++PairsIterator(table); }
EndIterator Pairs::end() { return EndIterator{}; }
