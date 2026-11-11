#pragma once
#include "../models/Activity.h"

namespace algogenie {
inline bool activityFeasible(const ActivityCase& input, const ActivityResult& result) {
    if (!input.valid() || result.count < 0 || result.count > input.size() || result.count > result.selected.size()) return false;
    IntArray seen(input.size());
    for (int i = 0; i < result.count; ++i) {
        int id = result.selected[i];
        if (id < 0 || id >= input.size() || seen[id]) return false;
        seen[id] = 1;
        if (i && input.finish[result.selected[i - 1]] > input.start[id]) return false;
    }
    return true;
}
// Independent prefix recurrence: OPT[k] = max(skip k, 1 + OPT[p(k)]).
// Unlike the featured DP, this describes a prefix, not a chain ending at i.
inline int activityOptimum(const ActivityCase& input) {
    ActivityOrder order(input); IntArray optimum(input.size() + 1);
    for (int k = 1; k <= input.size(); ++k) {
        int p = k - 1;
        while (p > 0 && input.finish[order.ids[p - 1]] > input.start[order.ids[k - 1]]) --p;
        int take = 1 + optimum[p];
        optimum[k] = take > optimum[k - 1] ? take : optimum[k - 1];
    }
    return optimum[input.size()];
}
inline bool activityVerified(const ActivityCase& input, const ActivityResult& result) {
    return activityFeasible(input, result) && result.count == activityOptimum(input);
}
}
