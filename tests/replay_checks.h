#pragma once
#include "../src/trace/SortReplay.h"

inline bool equalArrays(const IntArray& a, const IntArray& b) {
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); ++i) if (a[i] != b[i]) return false;
    return true;
}
inline bool equalEvents(const SortEvent& a, const SortEvent& b) {
    return a.kind == b.kind && a.begin == b.begin && a.middle == b.middle &&
        a.end == b.end && a.depth == b.depth && a.left == b.left &&
        a.right == b.right && a.index == b.index && a.value == b.value;
}
inline void testReplayCase(const IntArray& input) {
    IntArray traced(input), plain(input); SortTrace trace;
    algogenie::mergeSort(traced, &trace); algogenie::mergeSort(plain);
    check(equalArrays(traced, plain), "tracing leaves result unchanged");
    SortReplay replay(input, trace);
    replay.back(); check(replay.position() == 0, "back clamps at initial state");
    for (int k = 1; k <= trace.size(); ++k) {
        replay.next();
        IntArray values(replay.values), buffer(replay.buffer), valid(replay.bufferValid);
        replay.back(); replay.next();
        check(replay.position() == k && equalArrays(values, replay.values) &&
            equalArrays(buffer, replay.buffer) && equalArrays(valid, replay.bufferValid), "back then next restores full visible state");
        const SortEvent& event = trace[k - 1];
        if (event.kind == SortEventKind::Compare) {
            check(event.left >= event.begin && event.left < event.middle &&
                event.right >= event.middle && event.right < event.end, "comparison indices belong to their halves");
            const SortEvent& write = trace[k];
            const int l = replay.values[event.left], r = replay.values[event.right];
            check(write.kind == SortEventKind::BufferWrite && write.value == (l <= r ? l : r) &&
                write.left == (l <= r ? event.left : event.right), "recorded choice is correct, left wins ties");
        }
        if (event.kind == SortEventKind::CopyBack)
            check(replay.bufferValid[event.index] && replay.values[event.index] == replay.buffer[event.index], "copy back matches visible buffer");
    }
    check(replay.complete() && equalArrays(replay.values, traced), "complete history matches verified solver output");
    replay.next(); check(replay.position() == trace.size(), "next clamps at end");
    replay.restart();
    bool emptyBuffer = true;
    for (int i = 0; i < input.size(); ++i) if (replay.bufferValid[i]) emptyBuffer = false;
    check(equalArrays(replay.values, input) && !replay.current() && emptyBuffer, "restart restores original and hides buffer");
    SortTrace repeat; IntArray again(input); algogenie::mergeSort(again, &repeat);
    bool deterministic = repeat.size() == trace.size();
    for (int i = 0; deterministic && i < trace.size(); ++i) deterministic = equalEvents(trace[i], repeat[i]);
    check(deterministic, "repeat run has identical event sequence");
}
inline void replayChecks() {
    for (int n = 0; n <= 4; ++n) {
        int count = 1; for (int j = 0; j < n; ++j) count *= 3;
        for (int code = 0; code < count; ++code) {
            IntArray input(n); int digits = code;
            for (int j = 0; j < n; ++j) { input[j] = digits % 3 - 1; digits /= 3; }
            testReplayCase(input);
        }
    }
    IntArray input(30); for (int i = 0; i < 30; ++i) input[i] = 30 - i;
    testReplayCase(input); // Exceeds initial capacity; tests growable event storage.
    SortTrace tiny(3); IntArray sorted(input); algogenie::mergeSort(sorted, &tiny);
    SortReplay partial(input, tiny); partial.seek(tiny.size());
    check(tiny.truncated() && tiny.size() == 3 && !partial.complete(), "truncated trace never claims completion");
    check(isSorted(sorted) && preservesValues(input, sorted), "solver finishes after trace limit");
    SortTrace none(0); algogenie::mergeSort(sorted, &none);
    check(none.truncated() && none.size() == 0, "zero event capacity handled");
    IntArray singleton(1); algogenie::mergeSort(singleton, &tiny);
    check(!tiny.truncated() && tiny.size() == 2, "reusing recorder clears prior overflow");
}
