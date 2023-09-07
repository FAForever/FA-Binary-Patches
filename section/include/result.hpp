template <typename T>
struct Result
{
    T *object = nullptr;
    const char *reason = nullptr;

    constexpr static Result<T> Fail(const char *reason) { return {nullptr, reason}; }
    constexpr static Result<T> Success(void *data) { return {(T *)data, nullptr}; }

    inline bool IsFail() { return reason != nullptr; }
};