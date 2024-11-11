#include "moho.h"

extern const uintptr_t FIRST_CTOR;
extern const uintptr_t END_CTOR;

void InitCtors() {
    for(auto iter = &FIRST_CTOR; iter != &END_CTOR; ++iter)
        if (iter != nullptr)
            (reinterpret_cast<void(*)()>(*iter))();
}