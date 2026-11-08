#pragma once
#include "../src/app/SortComparison.h"

inline void comparisonChecks() {
    const double samples[] = {9, 1, 7, 3, 3, 5, 2};
    check(algogenie::medianOfSeven(samples) == 3 && samples[0] == 9, "median handles ties without changing samples");
    IntArray lesson(3); lesson[0] = 4; lesson[1] = 1; lesson[2] = 3;
    IntArray original(lesson);
    const auto report = algogenie::compareSorts(lesson);
    check(report.valid() && equalArrays(lesson, original), "comparison verifies both outputs and preserves original");
    check(report.merge.metrics.comparisons == 3 && report.merge.metrics.mainWrites == 5 &&
        report.merge.metrics.bufferWrites == 5 && report.merge.metrics.swaps == 0 &&
        report.merge.metrics.maximumDepth == 2, "hand-counted merge metrics for 4 1 3");
    check(report.quick.metrics.comparisons == 2 && report.quick.metrics.mainWrites == 4 &&
        report.quick.metrics.bufferWrites == 0 && report.quick.metrics.swaps == 2 &&
        report.quick.metrics.maximumDepth == 1, "hand-counted quick metrics for 4 1 3");
    check(report.merge.medianUs >= 0 && report.quick.medianUs >= 0 &&
        report.merge.medianUs == algogenie::medianOfSeven(report.merge.samplesUs), "valid measured median");
    IntArray ordered(8); for (int i = 0; i < 8; ++i) ordered[i] = i + 1;
    const auto sortedReport = algogenie::compareSorts(ordered);
    check(sortedReport.valid(), "sorted input comparison succeeds");
    check(sortedReport.merge.metrics.comparisons == 12 && sortedReport.merge.metrics.mainWrites == 24 &&
        sortedReport.merge.metrics.bufferWrites == 24 && sortedReport.merge.metrics.maximumDepth == 3,
        "balanced merge counts on eight sorted values");
    check(sortedReport.quick.metrics.comparisons == 28 && sortedReport.quick.metrics.swaps == 0 &&
        sortedReport.quick.metrics.mainWrites == 0 && sortedReport.quick.metrics.maximumDepth == 7,
        "last-pivot bad partitions and skipped self-swaps");
    IntArray empty; const auto emptyReport = algogenie::compareSorts(empty);
    check(emptyReport.valid() && emptyReport.merge.metrics.comparisons == 0 &&
        emptyReport.quick.metrics.maximumDepth == 0, "empty comparison resets counters");
    for (int n = 0; n <= 4; ++n) {
        int combinations = 1; for (int i = 0; i < n; ++i) combinations *= 3;
        for (int code = 0; code < combinations; ++code) {
            IntArray input(n); int digits = code;
            for (int i = 0; i < n; ++i) { input[i] = digits % 3 - 1; digits /= 3; }
            for (int algorithm = 0; algorithm < 2; ++algorithm) {
                IntArray plain(input), counted(input), traced(input);
                SortTrace trace; SortMetrics metrics, tracedMetrics;
                algogenie::runSort(plain, algorithm == 1);
                algogenie::runSort(counted, algorithm == 1, &metrics);
                if (algorithm == 1) algogenie::quickSort(traced, &trace, &tracedMetrics);
                else algogenie::mergeSort(traced, &trace, &tracedMetrics);
                check(equalArrays(plain, counted) && equalArrays(plain, traced), "trace/count switches preserve solver answer");
                long long comparisons = 0, writes = 0, buffers = 0, swaps = 0;
                for (int i = 0; i < trace.size(); ++i) {
                    const auto& event = trace[i];
                    if (event.kind == SortEventKind::Compare || event.kind == SortEventKind::ComparePivot) ++comparisons;
                    if (event.kind == SortEventKind::CopyBack) ++writes;
                    if (event.kind == SortEventKind::BufferWrite) ++buffers;
                    if ((event.kind == SortEventKind::PartitionSwap || event.kind == SortEventKind::PivotPlaced) && event.left != event.right) {
                        ++swaps; writes += 2;
                    }
                }
                check(metrics.comparisons == comparisons && metrics.mainWrites == writes &&
                    metrics.bufferWrites == buffers && metrics.swaps == swaps, "counts agree with independently inspected trace");
                check(metrics.comparisons == tracedMetrics.comparisons && metrics.mainWrites == tracedMetrics.mainWrites &&
                    metrics.bufferWrites == tracedMetrics.bufferWrites && metrics.maximumDepth == tracedMetrics.maximumDepth,
                    "recording does not change counts");
                IntArray zero;
                algogenie::runSort(zero, algorithm == 1, &metrics);
                check(metrics.comparisons == 0 && metrics.mainWrites == 0 && metrics.bufferWrites == 0 &&
                    metrics.swaps == 0 && metrics.maximumDepth == 0, "reused metrics reset completely");
            }
        }
    }
    for (int pattern = 0; pattern < 3; ++pattern) {
        IntArray input(100);
        for (int i = 0; i < input.size(); ++i) input[i] = pattern == 0 ? i : pattern == 1 ? 100 - i : 5;
        const auto boundary = algogenie::compareSorts(input);
        check(boundary.valid() && boundary.quick.metrics.comparisons == 4950,
            "maximum console input sorted/reversed/equal comparisons");
    }
}
