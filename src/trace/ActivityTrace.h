#pragma once
#include "../models/Activity.h"

namespace algogenie {
enum class ActivityEventKind { GreedyChoice, DPStart, DPCheck, DPUpdate, DPDone, Select, Finished };
struct ActivityEvent {
    ActivityEventKind kind;
    int i = -1, j = -1, value = 0;
    bool compatible = false;
};
class ActivityTrace {
    ActivityEvent* data_ = nullptr;
    int size_ = 0, capacity_ = 0, limit_;
    bool truncated_ = false;
public:
    explicit ActivityTrace(int limit = 20000) : limit_(limit < 0 ? 0 : limit) {}
    ActivityTrace(const ActivityTrace&) = delete;
    ActivityTrace& operator=(const ActivityTrace&) = delete;
    ~ActivityTrace() { delete[] data_; }
    void clear() { size_ = 0; truncated_ = false; }
    int size() const { return size_; }
    bool truncated() const { return truncated_; }
    const ActivityEvent& operator[](int i) const { assert(i >= 0 && i < size_); return data_[i]; }
    void add(ActivityEvent event) {
        if (size_ >= limit_) { truncated_ = true; return; }
        if (size_ == capacity_) {
            int next = capacity_ ? capacity_ * 2 : 64;
            if (next > limit_) next = limit_;
            ActivityEvent* grown = new ActivityEvent[next];
            for (int i = 0; i < size_; ++i) grown[i] = data_[i];
            delete[] data_; data_ = grown; capacity_ = next;
        }
        data_[size_++] = event;
    }
};
inline void activityRecord(ActivityTrace* trace, ActivityEvent event) { if (trace) trace->add(event); }
class ActivityReplay {
    const ActivityTrace& trace_;
    int position_ = 0;
public:
    IntArray best, predecessor, selected;
    ActivityReplay(int n, const ActivityTrace& trace) : trace_(trace), best(n), predecessor(n), selected(n) { seek(0); }
    int position() const { return position_; }
    const ActivityEvent* current() const { return position_ ? &trace_[position_ - 1] : nullptr; }
    bool complete() const { return !trace_.truncated() && current() && current()->kind == ActivityEventKind::Finished; }
    void seek(int target) {
        if (target < 0) target = 0;
        if (target > trace_.size()) target = trace_.size();
        for (int i = 0; i < best.size(); ++i) { best[i] = 0; predecessor[i] = -1; selected[i] = 0; }
        for (int k = 0; k < target; ++k) {
            const ActivityEvent& e = trace_[k];
            if (e.kind == ActivityEventKind::DPStart) best[e.i] = 1;
            if (e.kind == ActivityEventKind::DPUpdate) { best[e.i] = e.value; predecessor[e.i] = e.j; }
            if (e.kind == ActivityEventKind::Select ||
                (e.kind == ActivityEventKind::GreedyChoice && e.compatible)) selected[e.i] = 1;
        }
        position_ = target;
    }
    void next() { seek(position_ + 1); }
    void back() { seek(position_ - 1); }
    void restart() { seek(0); }
};
}
