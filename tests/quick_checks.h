#pragma once
#include "../src/algorithms/sorting/QuickSort.h"

inline void testQuickReplay(const IntArray& input) {
    IntArray sorted(input), untraced(input); SortTrace trace;
    algogenie::quickSort(sorted, &trace); algogenie::quickSort(untraced);
    check(isSorted(sorted) && preservesValues(input, sorted), "quicksort correctness");
    check(equalArrays(sorted, untraced), "quicksort tracing leaves result unchanged");
    SortReplay replay(input, trace);
    replay.back(); check(replay.position() == 0, "quick back at zero");
    for (int k = 1; k <= trace.size(); ++k) {
        replay.next(); const SortEvent& event = trace[k - 1];
        IntArray snapshot(replay.values);
        replay.back(); replay.next();
        check(equalArrays(replay.values, snapshot) && replay.position() == k, "quick next/back restores state");
        check(preservesValues(input, replay.values), "every quick frame preserves input multiset");
        if (event.kind == SortEventKind::ComparePivot) {
            bool invariant = event.begin <= event.middle && event.middle <= event.right && event.right < event.end - 1;
            for (int i = event.begin; i < event.middle; ++i) invariant = invariant && replay.values[i] <= event.value;
            for (int i = event.middle; i < event.right; ++i) invariant = invariant && replay.values[i] > event.value;
            check(invariant && replay.values[event.index] == event.value, "Lomuto loop invariant and pivot preserved");
            const SortEvent& decision = trace[k];
            check(decision.kind == (replay.values[event.right] <= event.value ?
                SortEventKind::PartitionSwap : SortEventKind::KeepRight), "comparison decision matches values");
        }
        if (event.kind == SortEventKind::PivotPlaced) {
            bool partitioned = replay.values[event.index] == event.value;
            for (int i = event.begin; i < event.index; ++i) partitioned = partitioned && replay.values[i] <= event.value;
            for (int i = event.index + 1; i < event.end; ++i) partitioned = partitioned && replay.values[i] > event.value;
            check(partitioned, "pivot placement partitions active range");
        }
    }
    check(replay.complete() && equalArrays(replay.values, sorted), "quick replay final state matches solver");
    replay.next(); check(replay.position() == trace.size(), "quick next clamps at end");
    replay.restart(); check(equalArrays(input, replay.values) && !replay.current(), "quick restart restores input");
    SortTrace repeat; IntArray again(input); algogenie::quickSort(again, &repeat);
    bool deterministic = trace.size() == repeat.size();
    for (int i = 0; deterministic && i < trace.size(); ++i) deterministic = equalEvents(trace[i], repeat[i]);
    check(deterministic, "quick repeat trace deterministic");
}

inline void quickChecks() {
    // Independent property checks for all short arrays over {-1,0,1}.
    for (int n = 0; n <= 7; ++n) {
        int combinations = 1; for (int i = 0; i < n; ++i) combinations *= 3;
        for (int code = 0; code < combinations; ++code) {
            IntArray input(n); int digits = code;
            for (int i = 0; i < n; ++i) { input[i] = digits % 3 - 1; digits /= 3; }
            IntArray result(input); algogenie::quickSort(result);
            check(isSorted(result) && preservesValues(input, result), "exhaustive quick correctness");
            if (n <= 4) testQuickReplay(input);
        }
    }
    const int inputValues[] = {INT_MAX, -2, 5, INT_MIN, 5, 0};
    const int expected[] = {INT_MIN, -2, 0, 5, 5, INT_MAX};
    IntArray input(6);
    for (int i = 0; i < 6; ++i) input[i] = inputValues[i];
    testQuickReplay(input); algogenie::quickSort(input);
    bool matches = true; for (int i = 0; i < 6; ++i) matches = matches && input[i] == expected[i];
    check(matches, "quick known answer including integer limits");
    for (int pattern = 0; pattern < 3; ++pattern) {
        IntArray worst(30);
        for (int i = 0; i < 30; ++i) worst[i] = pattern == 0 ? i : pattern == 1 ? 30 - i : 7;
        testQuickReplay(worst);
    }
    SortTrace shortTrace(2); IntArray original(3); original[0] = 4; original[1] = 1; original[2] = 3;
    IntArray sorted(original); algogenie::quickSort(sorted, &shortTrace);
    SortReplay partial(original, shortTrace); partial.seek(shortTrace.size());
    check(shortTrace.truncated() && !partial.complete() && isSorted(sorted), "quick finishes after recorder overflow");
    IntArray empty; algogenie::quickSort(empty, &shortTrace);
    check(!shortTrace.truncated() && shortTrace.size() == 2, "quick recorder reuse clears overflow");
}
