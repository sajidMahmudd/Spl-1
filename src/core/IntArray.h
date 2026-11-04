#pragma once
#include <cassert>

// Owns its allocation. Copying creates independent storage.
class IntArray {
    int* data_;
    int size_;
public:
    explicit IntArray(int size = 0) : data_(size > 0 ? new int[size]() : nullptr), size_(size) {
        assert(size >= 0);
    }
    IntArray(const IntArray& other) : IntArray(other.size_) {
        for (int i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }
    IntArray& operator=(const IntArray& other) {
        if (this != &other) {
            IntArray copy(other);
            int* oldData = data_; data_ = copy.data_; copy.data_ = oldData;
            int oldSize = size_; size_ = copy.size_; copy.size_ = oldSize;
        }
        return *this;
    }
    ~IntArray() { delete[] data_; }
    int size() const { return size_; }
    int& operator[](int index) { assert(index >= 0 && index < size_); return data_[index]; }
    const int& operator[](int index) const { assert(index >= 0 && index < size_); return data_[index]; }
};
