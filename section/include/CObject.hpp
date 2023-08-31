#include "moho.h"

void *GetCScriptType()
{
    return reinterpret_cast<void *(*)()>(0x4C8530)();
}

struct Ptr
{
    void *a;
    void *b;
};

Ptr REF_UpcastPtr(RRef *ref, void *sctype)
{
    Ptr p;
    reinterpret_cast<void *(__cdecl *)(Ptr *, RRef *, void *)>(0x8D9590)(&p, ref, sctype);
    return p;
}

void *GetCObject(lua_State *l, int index)
{
    lua_pushvalue(l, index);
    if (lua_istable(l, -1))
    {
        lua_pushstring(l, "_c_object");
        lua_rawget(l, -2);
    }
    if (lua_isuserdata(l, -1))
    {
        RRef udata = lua_touserdata(l, -1);
        void *sctype = GetCScriptType();
        Ptr p = REF_UpcastPtr(&udata, sctype);
        lua_pop(l, 1);
        return p.a;
    }
    lua_pop(l, 1);
    return nullptr;
}

struct Result
{
    void *object = nullptr;
    const char *reason = nullptr;

    constexpr static Result Fail(const char *reason) { return {nullptr, reason}; }
    constexpr static Result Success(void *data) { return {data, nullptr}; }
};

struct Obj
{
    void *a;
    char *b;
};

Obj CastObj(void *obj)
{
    Obj res;
    reinterpret_cast<void *(__cdecl *)(Obj *, void *)>(0x4C9030)(&res, obj);
    return res;
}

void *LookupRType(void *typeinfo)
{
    return reinterpret_cast<void *(__cdecl *)(void *)>(0x8E0750)(typeinfo);
}
template <int T, int TInfo>
class CScriptClass
{
public:
    const static int Type = T;
    const static int Info = TInfo;
};

class CPlatoon : public CScriptClass<0x10C6FCC, 0xF6A1FC>
{
};
class CUserUnit : public CScriptClass<0x10C77AC, 0xF881E0>
{
};
class CMAUIBitmap : public CScriptClass<0x10C7704, 0xF832F4>
{
};

template <class CScriptType>
Result GetCScriptObject(lua_State *l, int index)
{
    void **obj = GetCObject(l, index);
    if (obj == nullptr)
    {
        return Result::Fail("Expected a game object. (Did you call with '.' instead of ':'?)");
    }
    if (*obj == nullptr)
    {
        return Result::Fail("Game object has been destroyed");
    }
    Obj o = CastObj(*obj);

    void **type_ = (void **)CScriptType::Type;
    if (!*type_)
    {
        *type_ = LookupRType((void *)CScriptType::Info);
    }
    Ptr p = REF_UpcastPtr((RRef *)&o, *type_);
    if (p.a == nullptr)
    {
        return Result::Fail("Incorrect type of game object.  (Did you call with '.' instead of ':'?)");
    }
    return Result::Success(p.a);
}

void test()
{
    lua_State *l;
    GetCScriptObject<CPlatoon>(l, -1);
}