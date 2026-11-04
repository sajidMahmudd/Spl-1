#pragma once
#include "../../core/IntArray.h"

namespace algogenie {
inline void mergeRange(IntArray& values, IntArray& buffer, int begin, int end) {
    // Ranges are half-open: [begin, end).
    if (end - begin < 2) return;
    const int middle = begin + (end - begin) / 2;
    mergeRange(values, buffer, begin, middle);
    mergeRange(values, buffer, middle, end);
    int left = begin, right = middle, next = begin;
    while (left < middle && right < end) {
        // Choosing the left on equality preserves stable tie order.
        if (values[left] <= values[right]) buffer[next++] = values[left++];
        else buffer[next++] = values[right++];
    }
    while (left < middle) buffer[next++] = values[left++];
    while (right < end) buffer[next++] = values[right++];
    for (int i = begin; i < end; ++i) values[i] = buffer[i];
}
inline void mergeSort(IntArray& values) {
    IntArray buffer(values.size());
    mergeRange(values, buffer, 0, values.size());
}
}
