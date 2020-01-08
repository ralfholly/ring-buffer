#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdio>

#include "gmock/gmock.h"
using namespace testing;


namespace testing {
namespace ring_buffer {

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
        return head_ >= tail_ ?
            head_ - tail_ :
            BUFSIZE - (tail_ - head_);
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


TEST(ring_buffer, simple_instatiation) {
    ring_buffer<double, 10> rg;
    EXPECT_EQ(10U, rg.capacity());
    EXPECT_EQ(0U, rg.size());
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, simple_add_remove) {
    ring_buffer<double, 10> rg;
    rg.add(1.1);
    EXPECT_FALSE(rg.empty());
    EXPECT_EQ(1U, rg.size());
    EXPECT_EQ(1.1, rg.remove());
    EXPECT_TRUE(rg.empty());
    EXPECT_EQ(0U, rg.size());
}

TEST(ring_buffer, multi_add) {
    ring_buffer<int, 10> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    rg.add(4);
    rg.add(5);
    EXPECT_EQ(5U, rg.size());
    EXPECT_FALSE(rg.empty());
    EXPECT_EQ(1, rg.remove());
    EXPECT_EQ(2, rg.remove());
    EXPECT_EQ(3, rg.remove());
    EXPECT_EQ(4, rg.remove());
    EXPECT_EQ(5, rg.remove());
    EXPECT_EQ(0U, rg.size());
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, overflow1) {
    ring_buffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    rg.add(4);
    rg.add(5);
    EXPECT_EQ(5U, rg.size());
    // The last add nudged the tail such that the value 1 (the oldest value)
    // doesn't exist anymore in the ring buffer.
    EXPECT_EQ(1, rg.remove());
    EXPECT_EQ(4U, rg.size());
    EXPECT_EQ(2, rg.remove());
    EXPECT_EQ(3U, rg.size());
    EXPECT_EQ(3, rg.remove());
    EXPECT_EQ(2U, rg.size());
    EXPECT_EQ(4, rg.remove());
    EXPECT_EQ(1U, rg.size());
    EXPECT_EQ(5, rg.remove());
    EXPECT_EQ(0U, rg.size());
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, overflow2) {
    ring_buffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    rg.remove();
    rg.remove();
    rg.remove();
    rg.add(4);
    EXPECT_EQ(1U, rg.size());
    rg.add(5);
    EXPECT_EQ(2U, rg.size());
    rg.add(6);
    EXPECT_EQ(3U, rg.size());
    rg.add(7);
    EXPECT_EQ(4U, rg.size());
    rg.add(8);
    EXPECT_EQ(5U, rg.size());
    rg.add(9);
    EXPECT_EQ(5U, rg.size());
    rg.add(10);
    EXPECT_EQ(5U, rg.size());

    EXPECT_EQ(6, rg.remove());
    EXPECT_EQ(4U, rg.size());
    EXPECT_EQ(7, rg.remove());
    EXPECT_EQ(3U, rg.size());
    EXPECT_EQ(8, rg.remove());
    EXPECT_EQ(2U, rg.size());
    EXPECT_EQ(9, rg.remove());
    EXPECT_EQ(1U, rg.size());
    EXPECT_EQ(10, rg.remove());
    EXPECT_EQ(0U, rg.size());
}

TEST(ring_buffer, fill_refill) {
    ring_buffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    EXPECT_EQ(1, rg.remove());
    EXPECT_EQ(2, rg.remove());
    EXPECT_EQ(3, rg.remove());
    EXPECT_TRUE(rg.empty());
    rg.add(1);
    rg.add(2);
    rg.add(3);
    EXPECT_EQ(1, rg.remove());
    EXPECT_EQ(2, rg.remove());
    EXPECT_EQ(3, rg.remove());
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, clear) {
    ring_buffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    EXPECT_FALSE(rg.empty());
    rg.clear();
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, ____) {
    ring_buffer<int, 10> rb;

    rb.add(123);
    rb.add(42);
    rb.add(23);
    assert(rb.size() == 3);

    while (!rb.empty()) {
        std::cout << rb.remove() << std::endl;
    }
    assert(rb.size() == 0);
}

} // namespace ring_buffer
} // namespace testing
