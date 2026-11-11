#pragma once
#include "../../trace/ActivityTrace.h"

namespace algogenie {
// Solvers take a validated case and its shared finish-sorted order.
// Endpoints may touch: next.start >= previous.finish. No weights.
inline ActivityResult activityGreedy(const ActivityCase& input, const ActivityOrder& order,
    ActivityMetrics* metrics = nullptr, ActivityTrace* trace = nullptr) {
    assert(input.valid());
    if (metrics) *metrics = ActivityMetrics{};
    if (trace) trace->clear();
    ActivityResult result(input.size()); int last = -1;
    for (int i = 0; i < input.size(); ++i) {
        int id = order.ids[i];
        if (last >= 0 && metrics) ++metrics->compatibilityChecks;
        bool accept = last < 0 || input.start[id] >= input.finish[order.ids[last]];
        activityRecord(trace, {ActivityEventKind::GreedyChoice, i, last, 0, accept});
        if (accept) { result.selected[result.count++] = id; last = i; }
    }
    activityRecord(trace, {ActivityEventKind::Finished, -1, -1, result.count});
    return result;
}
inline ActivityResult activityDP(const ActivityCase& input, const ActivityOrder& order,
    ActivityMetrics* metrics = nullptr, ActivityTrace* trace = nullptr) {
    assert(input.valid());
    if (metrics) *metrics = ActivityMetrics{};
    if (trace) trace->clear();
    int n = input.size(), endpoint = -1;
    IntArray best(n), previous(n);
    ActivityResult result(n);
    for (int i = 0; i < n; ++i) {
        best[i] = 1; previous[i] = -1;
        activityRecord(trace, {ActivityEventKind::DPStart, i});
        for (int j = 0; j < i; ++j) {
            if (metrics) ++metrics->compatibilityChecks;
            bool compatible = input.finish[order.ids[j]] <= input.start[order.ids[i]];
            activityRecord(trace, {ActivityEventKind::DPCheck, i, j, best[j] + 1, compatible});
            if (compatible && best[j] + 1 > best[i]) {
                best[i] = best[j] + 1; previous[i] = j;
                if (metrics) ++metrics->dpUpdates;
                activityRecord(trace, {ActivityEventKind::DPUpdate, i, j, best[i], true});
            }
        }
        activityRecord(trace, {ActivityEventKind::DPDone, i, previous[i], best[i]});
        // On ties keep the earlier endpoint for deterministic reconstruction.
        if (endpoint < 0 || best[i] > best[endpoint]) endpoint = i;
    }
    if (endpoint >= 0) {
        result.count = best[endpoint]; int at = result.count;
        for (int i = endpoint; i >= 0; i = previous[i]) {
            result.selected[--at] = order.ids[i];
            activityRecord(trace, {ActivityEventKind::Select, i, previous[i]});
        }
    }
    activityRecord(trace, {ActivityEventKind::Finished, -1, -1, result.count});
    return result;
}
}
