#pragma once
#include "../core/IntArray.h"

inline bool isSorted(const IntArray& values) {
    for (int i = 1; i < values.size(); ++i)
        if (values[i - 1] > values[i]) return false;
    return true;
}
// Independent frequency checks, deliberately not another sorting algorithm.
// O(n^2); appropriate for this milestone's bounded console inputs.
inline bool preservesValues(const IntArray& before, const IntArray& after) {
    if (before.size() != after.size()) return false;
    for (int i = 0; i < before.size(); ++i) {
        int expected = 0, actual = 0;
        for (int j = 0; j < before.size(); ++j) {
            if (before[j] == before[i]) ++expected;
            if (after[j] == before[i]) ++actual;
        }
        if (expected != actual) return false;
    }
    return true;
}
