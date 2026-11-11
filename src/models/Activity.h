#pragma once
#include "../core/IntArray.h"

namespace algogenie {
constexpr int activityLimit = 100;
constexpr int activityLearnLimit = 20;
// Identity is the original input row + 1. Sorting changes indices, never IDs.
struct ActivityCase {
    IntArray start, finish;
    explicit ActivityCase(int n = 0) : start(n), finish(n) {}
    int size() const { return start.size(); }
    bool valid() const {
        if (size() > activityLimit || finish.size() != size()) return false;
        for (int i = 0; i < size(); ++i) if (start[i] >= finish[i]) return false;
        return true;
    }
};
struct ActivityOrder {
    IntArray ids;
    long long comparisons = 0;
    explicit ActivityOrder(const ActivityCase& input) : ids(input.size()) {
        // Stable handwritten insertion sort: finish, start, original ID.
        for (int i = 0; i < input.size(); ++i) ids[i] = i;
        for (int i = 1; i < ids.size(); ++i) {
            int id = ids[i], j = i;
            while (j > 0) {
                ++comparisons;
                int before = ids[j - 1];
                bool later = input.finish[before] > input.finish[id] ||
                    (input.finish[before] == input.finish[id] && input.start[before] > input.start[id]);
                if (!later) break;
                ids[j] = before; --j;
            }
            ids[j] = id;
        }
    }
};
struct ActivityMetrics {
    long long compatibilityChecks = 0;
    long long dpUpdates = 0; // Successful improvements, not initial best[i] = 1.
};
struct ActivityResult {
    IntArray selected; // Original row IDs (zero-based internally), chronological order.
    int count = 0;
    explicit ActivityResult(int n = 0) : selected(n) {}
};
}
