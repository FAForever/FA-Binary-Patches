
//Replaces Insert with set table
asm(
    ".section h0; .set h0,0x008BA4A9;" // at EntityCategoryFilterOut UI
    "call 0x009087A0;" // Call LuaObject::SetObject(int index, LuaObject& obj)
    ".section h1; .set h1,0x008BA177;" // at EntityCategoryFilterDown UI
    "call 0x009087A0;"// Call LuaObject::SetObject(int index, LuaObject& obj)
    ".section h2; .set h2,0x008BDB60;" // at SelectUnits
    "call 0x009087A0;"// Call LuaObject::SetObject(int index, LuaObject& obj)
    ".section h3; .set h3,0x00759E53;" // at EntityCategoryFilterDown Sim
    "call 0x009087A0;"// Call LuaObject::SetObject(int index, LuaObject& obj)
);

// Testing commands
// UI_Lua local _ti = table.insert local i = 1 local s = GetSelectedUnits() or {} _G.table.insert = function(...) LOG(i) i = i + 1 return _ti(unpack(arg)) end EntityCategoryFilterDown(categories.COMMAND,s) _G.table.insert = _ti
// UI_Lua local _ti = table.insert local i = 1 local s = GetSelectedUnits() or {} _G.table.insert = function(...) LOG(i) i = i + 1 return _ti(unpack(arg)) end EntityCategoryFilterOut(categories.COMMAND,s) _G.table.insert = _ti

