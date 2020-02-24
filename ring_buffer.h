#ifndef RING_BUFFER_H__
#define RING_BUFFER_H__

#include <cstddef>
#include <cassert>

template<typename T, int N>
class ring_buffer {
public:
    ring_buffer() { clear(); }
    size_t capacity() const { return N; }
    bool empty() const { return head_ == tail_; }
    void add(const T& item) {
        buffer_[head_] = item;
        advance(head_);
        if (head_ == tail_) {
            advance(tail_); // Drop oldest entry, keep rest.
        }
    }
    size_t size() const {
        return (head_ - tail_ + BUFSIZE) % BUFSIZE;
    }
    const T& remove() {
        assert(!empty());
        size_t old_tail = tail_;
        advance(tail_);
        return buffer_[old_tail];
    }
    void clear() { tail_ = head_ = 0U; }

private:
    static const size_t BUFSIZE = N + 1U;
    void advance(size_t& value) { value = (value + 1) % BUFSIZE; }

    T buffer_[BUFSIZE];
    size_t head_;
    size_t tail_;
};

#endif
