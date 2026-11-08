#pragma once
#include "../../core/IntArray.h"
#include "../../trace/SortTrace.h"

namespace algogenie {
inline void mergeRange(IntArray& values, IntArray& buffer, int begin, int end, SortTrace* trace, int depth) {
    // Ranges are half-open: [begin, end).
    const int middle = begin + (end - begin) / 2;
    if (end - begin < 2) {
        record(trace, {SortEventKind::BaseCase, begin, middle, end, depth});
        return;
    }
    record(trace, {SortEventKind::Split, begin, middle, end, depth});
    mergeRange(values, buffer, begin, middle, trace, depth + 1);
    mergeRange(values, buffer, middle, end, trace, depth + 1);
    record(trace, {SortEventKind::BeginMerge, begin, middle, end, depth});
    int left = begin, right = middle, next = begin;
    while (left < middle && right < end) {
        // Choosing the left on equality preserves stable tie order.
        record(trace, {SortEventKind::Compare, begin, middle, end, depth, left, right});
        int source;
        if (values[left] <= values[right]) source = left++;
        else source = right++;
        buffer[next] = values[source];
        record(trace, {SortEventKind::BufferWrite, begin, middle, end, depth, source, -1, next, buffer[next]});
        ++next;
    }
    while (left < middle || right < end) {
        int source = left < middle ? left++ : right++;
        buffer[next] = values[source];
        record(trace, {SortEventKind::BufferWrite, begin, middle, end, depth, source, -1, next, buffer[next]});
        ++next;
    }
    for (int i = begin; i < end; ++i) {
        values[i] = buffer[i];
        record(trace, {SortEventKind::CopyBack, begin, middle, end, depth, -1, -1, i, values[i]});
    }
    record(trace, {SortEventKind::MergeDone, begin, middle, end, depth});
}
inline void mergeSort(IntArray& values, SortTrace* trace = nullptr) {
    if (trace) trace->clear();
    IntArray buffer(values.size());
    mergeRange(values, buffer, 0, values.size(), trace, 0);
    record(trace, {SortEventKind::Finished, 0, values.size() / 2, values.size(), 0});
}
}
