#pragma once
#include "../../core/IntArray.h"
#include "../../trace/SortTrace.h"
#include "../../models/SortMetrics.h"

namespace algogenie {
// Lomuto partition, fixed last-element pivot, half-open ranges [begin,end).
inline int partitionLast(IntArray& values, int begin, int end, SortTrace* trace, int depth, SortMetrics* metrics) {
    const int pivotIndex = end - 1;
    const int pivot = values[pivotIndex];
    int boundary = begin;
    record(trace, {SortEventKind::ChoosePivot, begin, boundary, end, depth, -1, -1, pivotIndex, pivot});
    for (int scan = begin; scan < pivotIndex; ++scan) {
        // Invariant: [begin,boundary) <= pivot; [boundary,scan) > pivot.
        record(trace, {SortEventKind::ComparePivot, begin, boundary, end, depth, boundary, scan, pivotIndex, pivot});
        if (metrics) ++metrics->comparisons;
        if (values[scan] <= pivot) {
            // A self-swap needs no writes, but still advances the partition boundary.
            if (boundary != scan) {
                const int temporary = values[boundary];
                values[boundary] = values[scan];
                values[scan] = temporary;
                if (metrics) { ++metrics->swaps; metrics->mainWrites += 2; }
            }
            record(trace, {SortEventKind::PartitionSwap, begin, boundary + 1, end, depth, boundary, scan, pivotIndex, pivot});
            ++boundary;
        } else {
            record(trace, {SortEventKind::KeepRight, begin, boundary, end, depth, boundary, scan, pivotIndex, pivot});
        }
    }
    if (boundary != pivotIndex) {
        const int temporary = values[boundary];
        values[boundary] = values[pivotIndex];
        values[pivotIndex] = temporary;
        if (metrics) { ++metrics->swaps; metrics->mainWrites += 2; }
    }
    record(trace, {SortEventKind::PivotPlaced, begin, boundary, end, depth, boundary, pivotIndex, boundary, pivot});
    return boundary;
}

inline void quickRange(IntArray& values, int begin, int end, SortTrace* trace, int depth, SortMetrics* metrics) {
    noteDepth(metrics, depth);
    if (end - begin < 2) {
        record(trace, {SortEventKind::BaseCase, begin, begin, end, depth});
        return;
    }
    const int pivot = partitionLast(values, begin, end, trace, depth, metrics);
    // The pivot is already in its final place; exclude it from both recursive calls.
    quickRange(values, begin, pivot, trace, depth + 1, metrics);
    quickRange(values, pivot + 1, end, trace, depth + 1, metrics);
    record(trace, {SortEventKind::PartitionDone, begin, pivot, end, depth});
}

inline void quickSort(IntArray& values, SortTrace* trace = nullptr, SortMetrics* metrics = nullptr) {
    if (trace) trace->clear();
    if (metrics) *metrics = SortMetrics{};
    quickRange(values, 0, values.size(), trace, 0, metrics);
    record(trace, {SortEventKind::Finished, 0, 0, values.size(), 0});
}
}
