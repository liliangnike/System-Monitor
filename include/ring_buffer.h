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

    void push(const T& value)
    {
        buf_[tail_] = value;
        tail_ = (tail_ + 1) % N;    // Circular buffer, when tail_ is N -1, it will become 0 (back to the beginning)
        if (full()) {
            head_ = (head_ + 1) % N;
        }
        else {
            ++count_;
        }
    }

    // std::move syntax version to support right value
    // The function does not accept left value
    // If you want to merge the 2 push functions into one, the function could be 'void push(T value)' that can accept both left and right values
    void push(T&& value)
    {
        buf_[tail_] = std::move(value);
        tail_ = (tail_ + 1) % N;    // Circular buffer, when tail_ is N -1, it will become 0 (back to the beginning)
        if (full()) {
            head_ = (head_ + 1) % N;
        }
        else {
            ++count_;
        }
    }

    // element access operator implementation
    // return the element reference
    T& operator[](std::size_t idx)
    {
        if (idx > count_) throw std::out_of_range("RingBuffer: index out of range");
        return buf_[(head_ + idx) % N];
    }

    const T& operator[](std::size_t idx) const
    {
        if (idx > count_) throw std::out_of_range("RingBuffer: index out of range");
        return buf_[(head_ + idx) % N];
    }

    const T& latest() const
    {
        if (count_ == 0) throw std::out_of_range("RingBuffer: empty");
        return (*this)[count_ - 1];
    }

    constexpr std::size_t capacity() const { return N; }
    std::size_t size() const { return count_; }
    bool empty() const { return count_ == 0; }
    bool full() const { return count_ == N; }
private:
    // C++ array is similar to C array, it is static and allocated in stack
    // vector is dynamic and allows to insert/delete element. Allocated in heap.
    //
    // Question: 'std::array<T, N> buf_' is same as 'T buf_[N]'?
    // 1. std::array (buf_) is real object, always takes its length; raw array is converted into pointer when it is being passed to function, length will be lost
    // 2. std::array provides standard STL functions like .begin(), .end(), .size(), .at(). It makes template class to be more generic
    // 3. std::array .at() function can throw exceptions. Raw array will cause undefine behaviors when it is overflow
    // 4. std::array supports to be copied/assigned. Raw array could not do this.
    // 5. std::array<T, 0> is also valid, but T buf_[0] is invalid
    std::array<T, N> buf_;
    std::size_t head_;      // oldest element in the circular buffer
    std::size_t tail_;      // position of next element to be written in
    std::size_t count_;     // quantity of all elements
};

#endif
