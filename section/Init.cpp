#include "moho.h"

typedef void (*global_ctor)();
extern const global_ctor FIRST_CTOR;
extern const global_ctor END_CTOR;

void InitCtors()
{
    for (auto iter = &FIRST_CTOR; iter != &END_CTOR; ++iter)
        if (iter != nullptr)
            (*iter)();
}