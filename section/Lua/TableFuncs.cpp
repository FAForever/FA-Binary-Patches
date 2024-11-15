#include <LuaAPI.h>
namespace lua
{
    struct TObject
    {
        int tt;
        void *value;
    };

    struct Node
    {
        TObject i_key;
        TObject i_val;
        Node *next;
    };

    struct Table
    {
        /* lua::GCObject*/ void *next;
        uint8_t tt;
        uint8_t marked;
        uint16_t gap;
        uint8_t flags;
        uint8_t lsizenode;
        // padding byte
        // padding byte
        Table *metatable;
        TObject *array;
        lua::Node *node;
        lua::Node *firstfree;
        /*lua::GCObject*/ void *gclist;
        int sizearray;
    };

} // namespace lua

VALIDATE_SIZE(lua::Table, 0x24);

int lua_tablesize(lua_State *L)
{
    int size = 0;
    auto base = GetField<lua::TObject *>(L, 0xC);
    auto top = GetField<lua::TObject *>(L, 0x8);
    if (base >= top || base->tt != LUA_TTABLE)
    {
        lua_pushnumber(L, size);
        return 1;
    }

    auto table = (lua::Table *)base->value;
    uint8_t lsizenode = table->lsizenode;
    if (lsizenode)
    {
        uint32_t num_nodes = 1 << lsizenode;
        auto node = table->node;
        do
        {
            if (node->i_val.tt)
                ++size;
            ++node;
            --num_nodes;
        } while (num_nodes);
    }
    int sizearray = table->sizearray;
    if (sizearray)
    {
        auto array = table->array;
        do
        {
            if (array->tt)
                ++size;
            ++array;
            --sizearray;
        } while (sizearray);
    }

    lua_pushnumber(L, size);
    return 1;
}

int lua_tableempty(lua_State *L)
{
    auto base = GetField<lua::TObject *>(L, 0xC);
    auto top = GetField<lua::TObject *>(L, 0x8);
    if (base >= top || base->tt != LUA_TTABLE)
    {
        lua_pushboolean(L, true);
        return 1;
    }

    auto table = (lua::Table *)base->value;
    constexpr auto has_nodes = [](lua::Table *t)
    {
        uint8_t lsizenode = t->lsizenode;
        if (!lsizenode)
            return false;

        uint32_t num_nodes = 1 << lsizenode;
        auto node = t->node;
        while (node->i_val.tt == LUA_TNIL)
        {
            ++node;
            if (!--num_nodes)
                return false;
        }
        return true;
    };

    constexpr auto has_array = [](lua::Table *t)
    {
        int sizearray = t->sizearray;
        if (!sizearray)
            return false;

        auto array = t->array;
        while (array->tt == LUA_TNIL)
        {
            ++array;
            if (!--sizearray)
                return false;
        }
        return true;
    };

    lua_pushboolean(L, !(has_nodes(table) || has_array(table)));
    return 1;
}

int TableClone(lua_State *L)
{
    LuaObject obj{L->LuaState, 1};
    LuaObject cloned{};
    obj.Clone(&cloned);
    cloned.PushStack(L);
    return 1;
}

// UI_Lua reprsl({table.unpack2({1,2,3,4},2,3)})
// UI_Lua reprsl({table.unpack2({1,2,3,4},2)})
// UI_Lua reprsl({table.unpack2({1,2,3,4})})
// UI_Lua reprsl({table.unpack2({1,2,3,4},-1)})
// UI_Lua reprsl({table.unpack2({1,2,3,4},-1000)})
// UI_Lua reprsl({table.unpack2({1,2,3,4},0, 1000000)}) //stack overflow

int lua_unpack(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TTABLE);
    const int n = lua_getn(l, 1);
    const int start_i = luaL_optnumber(l, 2, 1);
    const int end_i = luaL_optnumber(l, 3, n);
    if (start_i > end_i)
        return 0;

    const int n_stack = end_i - start_i + 1;
    luaL_checkstack(l, n_stack, "too many results to unpack");
    for (int i = start_i; i <= end_i; i++)
    {
        lua_rawgeti(l, 1, i);
    }
    return n_stack;
}

const luaL_reg RegTableFuncsDesc[] = {{"getsize2", &lua_tablesize},
                                      {"empty2", &lua_tableempty},
                                      {"getn2", (lua_CFunction)0x00927C20},
                                      {"clone", &TableClone},
                                      {"unpack", &lua_unpack},
                                      {nullptr, nullptr}};

extern const luaL_reg original_table_funcs[] asm("0x00D47418");

int __cdecl lua_openlibtable(lua_State *L)
{
    luaL_openlib(L, "table", original_table_funcs, 0);
    luaL_openlib(L, "table", RegTableFuncsDesc, 0);
    return 1;
}