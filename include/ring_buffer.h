#ifndef __RING_BUFFER_HEADER__
#define __RING_BUFFER_HEADER__

#include <array>

// C++ generic programming

template <typename T, std::size_t N>
class RingBuffer {
    // static_assert is very important in generic programming
    // static_assert and assert is similar to constexpr and const
    //
    // static_assert takes effect during compile-time; assert is in program runtime
    // statit_assert accepts constexpr type; assert accepts any type expression
    //
    // If you want all errors to be found during compilation, just use static_assert
    static_assert(N > 0, "RingBuffer size must be > 0.");
public:
    RingBuffer() : head_(0), tail_(0), count_(0) {}

private:
    // C++ array is similar to C array, it is static and allocated in stack
    // vector is dynamic and allows to insert/delete element. Allocated in heap.
    std::array<T, N> buf_;
    std::size_t head_;
    std::size_t tail_;
    std::size_t count_;
};

#endif
