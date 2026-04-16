#include <new>

void *operator new(std::size_t n, const std::nothrow_t &tag) noexcept
{
    return malloc(n);
}

void *operator new[](std::size_t n, const std::nothrow_t &tag) noexcept
{
    return malloc(n);
}

void *operator new(std::size_t n) noexcept(false)
{
    return shi_new(n);
}
void *operator new[](std::size_t n) noexcept(false)
{
    return shi_new(n);
}
void operator delete(void *p) noexcept
{
    free(p);
}
void operator delete[](void *p) noexcept
{
    free(p);
}

void operator delete(void *p, const std::nothrow_t &tag) noexcept
{
    free(p);
}
void operator delete[](void *p, const std::nothrow_t &tag) noexcept
{
    free(p);
}

void operator delete(void *p, unsigned int) noexcept
{
    free(p);
}
void operator delete[](void *p, unsigned int) noexcept
{
    free(p);
}

const std::nothrow_t std::nothrow{};