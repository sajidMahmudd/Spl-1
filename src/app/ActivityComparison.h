#pragma once
#include "SortComparison.h" // Shared seven-sample median and steady-clock facilities.
#include "../algorithms/activity/ActivitySelection.h"
#include "../verify/ActivityVerify.h"

namespace algogenie {
struct ActivityMeasurement {
    ActivityResult result;
    ActivityMetrics metrics;
    double samples[timingRepetitions] = {}, medianUs = 0;
    bool timedValid = true;
};
struct ActivityReport {
    ActivityReport() {} // Default-initialize owned arrays without aggregate-conversion warnings.
    bool available = false, comparison = false, learning = false, dp = false;
    ActivityCase input;
    ActivityMeasurement first, second;
    long long preparationComparisons = 0;
    int traceEvents = 0;
    bool traceTruncated = false;
    bool valid() const {
        if (!available || !input.valid() || !activityVerified(input, first.result)) return false;
        return !comparison || (activityVerified(input, second.result) && first.result.count == second.result.count &&
            first.timedValid && second.timedValid);
    }
};
inline ActivityResult solveActivity(const ActivityCase& input, const ActivityOrder& order, bool dp,
    ActivityMetrics* metrics = nullptr, ActivityTrace* trace = nullptr) {
    return dp ? activityDP(input, order, metrics, trace) : activityGreedy(input, order, metrics, trace);
}
inline ActivityReport runActivity(const ActivityCase& input, bool dp, ActivityTrace* trace = nullptr) {
    ActivityReport report;
    if (!input.valid()) return report;
    report.input = input; report.dp = dp; report.learning = trace != nullptr;
    ActivityOrder order(input); report.preparationComparisons = order.comparisons;
    report.first.result = solveActivity(input, order, dp, &report.first.metrics, trace);
    report.available = true;
    if (trace) { report.traceEvents = trace->size(); report.traceTruncated = trace->truncated(); }
    return report;
}
inline void timeActivity(const ActivityCase& input, const ActivityOrder& order, bool dp,
    ActivityMeasurement& measurement, int trial, int optimum) {
    const auto begin = std::chrono::steady_clock::now();
    ActivityResult result = solveActivity(input, order, dp);
    const auto end = std::chrono::steady_clock::now();
    measurement.samples[trial] = std::chrono::duration<double, std::micro>(end - begin).count();
    // Returned result destruction and all verification are outside the timed call.
    if (!activityFeasible(input, result) || result.count != optimum) measurement.timedValid = false;
}
inline ActivityReport compareActivities(const ActivityCase& input) {
    ActivityReport report;
    if (!input.valid()) return report;
    report.available = true; report.comparison = true; report.input = input;
    ActivityOrder order(input); report.preparationComparisons = order.comparisons;
    report.first.result = solveActivity(input, order, false, &report.first.metrics);
    report.second.result = solveActivity(input, order, true, &report.second.metrics);
    if (!report.valid()) return report;
    int optimum = activityOptimum(input);
    auto greedyWarm = solveActivity(input, order, false), dpWarm = solveActivity(input, order, true);
    report.first.timedValid = activityFeasible(input, greedyWarm) && greedyWarm.count == optimum;
    report.second.timedValid = activityFeasible(input, dpWarm) && dpWarm.count == optimum;
    for (int trial = 0; trial < timingRepetitions; ++trial) {
        if (trial % 2 == 0) {
            timeActivity(input, order, false, report.first, trial, optimum);
            timeActivity(input, order, true, report.second, trial, optimum);
        } else {
            timeActivity(input, order, true, report.second, trial, optimum);
            timeActivity(input, order, false, report.first, trial, optimum);
        }
    }
    report.first.medianUs = medianOfSeven(report.first.samples);
    report.second.medianUs = medianOfSeven(report.second.samples);
    return report;
}
}
