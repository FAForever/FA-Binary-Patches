#pragma once
#include "moho.h"
#include "strings.h"

void *GetCScriptType()
{
    return reinterpret_cast<void *(*)()>(0x4C8530)();
}

RRef REF_UpcastPtr(RRef *ref, void *sctype)
{
    RRef p;
    reinterpret_cast<void *(__cdecl *)(RRef *, RRef *, void *)>(0x8D9590)(&p, ref, sctype);
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
        RRef p = REF_UpcastPtr(&udata, sctype);
        lua_pop(l, 1);
        return p.d;
    }
    lua_pop(l, 1);
    return nullptr;
}
template <typename T>
struct Result
{
    T *object = nullptr;
    const char *reason = nullptr;

    constexpr static Result<T> Fail(const char *reason) { return {nullptr, reason}; }
    constexpr static Result<T> Success(void *data) { return {(T *)data, nullptr}; }
};

RRef CastObj(void *obj)
{
    RRef res;
    reinterpret_cast<void *(__cdecl *)(RRef *, void *)>(0x4C9030)(&res, obj);
    return res;
}

void *LookupRType(void *typeinfo)
{
    return reinterpret_cast<void *(__cdecl *)(void *)>(0x8E0750)(typeinfo);
}

struct CPlatoon : public CScriptObject
{
    using Type = ObjectType<0x10C6FCC, 0xF6A1FC>;
};
struct CUserUnit : public CScriptObject
{
    using Type = ObjectType<0x10C77AC, 0xF881E0>;
};
struct CMauiBitmap : public CMauiControl
{
    using Type = ObjectType<0x10C7704, 0xF832F4>;
};

template <class CScriptClass>
Result<CScriptClass> GetCScriptObject(lua_State *l, int index)
{
    using CScriptType = typename CScriptClass::Type;
    using TResult = Result<CScriptClass>;
    void **obj = GetCObject(l, index);
    if (obj == nullptr)
    {
        return TResult::Fail(s_ExpectedAGameObject);
    }
    if (*obj == nullptr)
    {
        return TResult::Fail(s_GameObjectHasBeenDestroyed);
    }
    RRef o = CastObj(*obj);

    void **type_ = (void **)CScriptType::Type;
    if (!*type_)
    {
        *type_ = LookupRType((void *)CScriptType::Info);
    }
    RRef p = REF_UpcastPtr(&o, *type_);
    if (p.d == nullptr)
    {
        return TResult::Fail(s_IncorrectTypeOfGameObject);
    }
    return TResult::Success(p.d);
}