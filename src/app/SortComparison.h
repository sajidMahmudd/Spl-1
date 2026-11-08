#pragma once
#include <chrono>
#include "../algorithms/sorting/MergeSort.h"
#include "../algorithms/sorting/QuickSort.h"
#include "../verify/Sorting.h"

namespace algogenie {
constexpr int timingRepetitions = 7;
struct SortMeasurement {
    IntArray result;
    SortMetrics metrics;
    bool ordered = false, preserved = false, timedResultsValid = true;
    double samplesUs[timingRepetitions] = {};
    double medianUs = 0;
};
struct SortComparison {
    SortMeasurement merge, quick;
    bool equalOutputs = false;
    bool valid() const {
        return merge.ordered && merge.preserved && quick.ordered && quick.preserved &&
            merge.timedResultsValid && quick.timedResultsValid && equalOutputs;
    }
};
inline bool sameArray(const IntArray& a, const IntArray& b) {
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); ++i) if (a[i] != b[i]) return false;
    return true;
}
inline void runSort(IntArray& data, bool quick, SortMetrics* metrics = nullptr) {
    if (quick) quickSort(data, nullptr, metrics);
    else mergeSort(data, nullptr, metrics);
}
inline void countSort(const IntArray& original, bool quick, SortMeasurement& measurement) {
    measurement.result = original;
    runSort(measurement.result, quick, &measurement.metrics);
    measurement.ordered = isSorted(measurement.result);
    measurement.preserved = preservesValues(original, measurement.result);
}
inline double medianOfSeven(const double* samples) {
    double sorted[timingRepetitions];
    for (int i = 0; i < timingRepetitions; ++i) sorted[i] = samples[i];
    // Small handwritten insertion sort for seven measurements only.
    for (int i = 1; i < timingRepetitions; ++i) {
        const double value = sorted[i]; int j = i;
        while (j > 0 && sorted[j - 1] > value) { sorted[j] = sorted[j - 1]; --j; }
        sorted[j] = value;
    }
    return sorted[timingRepetitions / 2];
}
inline void timeOne(const IntArray& original, bool quick, SortMeasurement& measurement, int trial) {
    IntArray working(original); // Input copy is outside the timer.
    const auto begin = std::chrono::steady_clock::now();
    runSort(working, quick); // No trace recorder or metrics collector.
    const auto end = std::chrono::steady_clock::now();
    measurement.samplesUs[trial] = std::chrono::duration<double, std::micro>(end - begin).count();
    // Verify outside the timer; also consume outputs of each timed computation.
    if (!isSorted(working) || !preservesValues(original, working) || !sameArray(working, measurement.result))
        measurement.timedResultsValid = false;
}
inline SortComparison compareSorts(const IntArray& original) {
    SortComparison report;
    countSort(original, false, report.merge);
    countSort(original, true, report.quick);
    report.equalOutputs = sameArray(report.merge.result, report.quick.result);
    if (!report.valid()) return report;
    // One untimed warm-up per algorithm, also on fresh inputs.
    IntArray mergeWarm(original), quickWarm(original);
    runSort(mergeWarm, false); runSort(quickWarm, true);
    report.merge.timedResultsValid = sameArray(mergeWarm, report.merge.result);
    report.quick.timedResultsValid = sameArray(quickWarm, report.quick.result);
    for (int trial = 0; trial < timingRepetitions; ++trial) {
        // Alternate which algorithm runs first to reduce ordering bias.
        if (trial % 2 == 0) {
            timeOne(original, false, report.merge, trial);
            timeOne(original, true, report.quick, trial);
        } else {
            timeOne(original, true, report.quick, trial);
            timeOne(original, false, report.merge, trial);
        }
    }
    report.merge.medianUs = medianOfSeven(report.merge.samplesUs);
    report.quick.medianUs = medianOfSeven(report.quick.samplesUs);
    return report;
}
}
