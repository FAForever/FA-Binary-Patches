
#include "luaV_settable.h"

static void callTM(
    lua::lua_State *L,
    const lua::TObject *f,
    const lua::TObject *p1,
    const lua::TObject *p2,
    const lua::TObject *p3)
{
    setobj(L->top, f);      /* push function */
    setobj(L->top + 1, p1); /* 1st argument */
    setobj(L->top + 2, p2); /* 2nd argument */
    setobj(L->top + 3, p3); /* 3th argument */

    luaD_checkstack(L, 4); /* cannot check before (could invalidate p1...p3) */
    L->top += 4;
    luaD_call(L, L->top - 4, 0);
}

SHARED void __cdecl luaV_settable_fix(lua::lua_State *L, const lua::TObject *t, const lua::TObject *key, const lua::TObject *val)
{
    lua::TObject temp;
    for (int loop = 0; loop < MAXTAGLOOP; loop++)
    {
        const lua::TObject *tm;
        if (ttistable(t))
        { /* `t' is a table? */
            lua::Table *h = (lua::Table *)t->value;
            lua::TObject *oldval = luaH_set(L, h, key); /* do a primitive set */
            if (!ttisnil(oldval) ||                     /* result is no nil? */
                (tm = fasttm(L, h->metatable, lua::TM_NEWINDEX)) == NULL)
            { /* or no TM? */
                setobj(oldval, val);
                return;
            }
            /* else will try the tag method */
        }
        else if (ttisnil(tm = luaT_gettmbyobj(L, t, lua::TM_NEWINDEX)))
            luaG_typeerror(L, t, "index");
        if (ttisfunction(tm))
        {
            callTM(L, tm, t, key, val);
            return;
        }
        /* else repeat with `tm' */
        setobj(&temp, tm); /* avoid pointing inside table (may rehash) */
        t = &temp;
    }
    luaG_runerror(L, "loop in settable");
}
