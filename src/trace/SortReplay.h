#pragma once
#include "SortTrace.h"
#include "../core/IntArray.h"

class SortReplay {
    IntArray initial_;
    const SortTrace& trace_; // Borrows the trace; it must outlive the viewer.
    int position_ = 0;
public:
    IntArray values, buffer, bufferValid;
    SortReplay(const IntArray& initial, const SortTrace& trace)
        : initial_(initial), trace_(trace), values(initial), buffer(initial.size()), bufferValid(initial.size()) {}
    int position() const { return position_; }
    const SortEvent* current() const { return position_ == 0 ? nullptr : &trace_[position_ - 1]; }
    bool complete() const {
        return !trace_.truncated() && current() && current()->kind == SortEventKind::Finished;
    }
    // Position is the number of applied events. Zero means the original input.
    void seek(int target) {
        if (target < 0) target = 0;
        if (target > trace_.size()) target = trace_.size();
        values = initial_;
        for (int i = 0; i < values.size(); ++i) { buffer[i] = 0; bufferValid[i] = 0; }
        for (int i = 0; i < target; ++i) {
            const SortEvent& event = trace_[i];
            if (event.kind == SortEventKind::BeginMerge) {
                for (int j = 0; j < values.size(); ++j) bufferValid[j] = 0;
            } else if (event.kind == SortEventKind::BufferWrite) {
                buffer[event.index] = event.value; bufferValid[event.index] = 1;
            } else if (event.kind == SortEventKind::CopyBack) {
                values[event.index] = event.value;
            } else if (event.kind == SortEventKind::PartitionSwap || event.kind == SortEventKind::PivotPlaced) {
                const int temporary = values[event.left];
                values[event.left] = values[event.right];
                values[event.right] = temporary;
            }
        }
        position_ = target;
    }
    void next() { seek(position_ + 1); }
    void back() { seek(position_ - 1); }
    void restart() { seek(0); }
};
