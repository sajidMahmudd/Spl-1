#pragma once

struct SortMetrics {
    long long comparisons = 0; // Only comparisons between keys, not loop conditions.
    long long mainWrites = 0; // Assignments into the working input array.
    long long bufferWrites = 0; // Assignments into Merge Sort's merge buffer.
    long long swaps = 0; // Exchanges at distinct indices; each contributes two main writes.
    int maximumDepth = 0; // Root depth 0, including base-case calls.
};
inline void noteDepth(SortMetrics* metrics, int depth) {
    if (metrics && depth > metrics->maximumDepth) metrics->maximumDepth = depth;
}
