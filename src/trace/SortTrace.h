#pragma once
#include <cassert>

enum class SortEventKind {
    Split, BaseCase, BeginMerge, Compare, BufferWrite, CopyBack, MergeDone, Finished,
    ChoosePivot, ComparePivot, PartitionSwap, KeepRight, PivotPlaced, PartitionDone
};
struct SortEvent {
    // Merge: middle is the split. Quick partition: middle is the <= boundary.
    // Quick events: index/value identify the pivot; left/right identify operands.
    SortEventKind kind;
    int begin, middle, end, depth;
    int left = -1, right = -1, index = -1, value = 0;
};

// Handwritten growable storage. Copying an owning recorder is prohibited.
class SortTrace {
    SortEvent* events_ = nullptr;
    int count_ = 0, capacity_ = 0, limit_;
    bool truncated_ = false;
public:
    explicit SortTrace(int limit = 20000) : limit_(limit) { assert(limit >= 0); }
    SortTrace(const SortTrace&) = delete;
    SortTrace& operator=(const SortTrace&) = delete;
    ~SortTrace() { delete[] events_; }
    void clear() { count_ = 0; truncated_ = false; }
    void add(const SortEvent& event) {
        if (count_ >= limit_) { truncated_ = true; return; }
        if (count_ == capacity_) {
            int capacity = capacity_ == 0 ? 64 : capacity_ * 2;
            if (capacity > limit_) capacity = limit_;
            SortEvent* replacement = new SortEvent[capacity];
            for (int i = 0; i < count_; ++i) replacement[i] = events_[i];
            delete[] events_; events_ = replacement; capacity_ = capacity;
        }
        events_[count_++] = event;
    }
    int size() const { return count_; }
    bool truncated() const { return truncated_; }
    const SortEvent& operator[](int index) const {
        assert(index >= 0 && index < count_); return events_[index];
    }
};
inline void record(SortTrace* trace, const SortEvent& event) { if (trace) trace->add(event); }
