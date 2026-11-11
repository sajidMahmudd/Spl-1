#pragma once
#include "../src/algorithms/activity/ActivitySelection.h"
#include "../src/verify/ActivityVerify.h"

// Independent tiny oracle: enumerate subsets and reject every overlapping pair.
inline int bruteActivity(const algogenie::ActivityCase& input) {
    int best = 0;
    for (unsigned mask = 0; mask < (1u << input.size()); ++mask) {
        int count = 0; bool valid = true;
        for (int i = 0; i < input.size(); ++i) if (mask & (1u << i)) {
            ++count;
            for (int j = 0; j < i; ++j) if (mask & (1u << j))
                if (!(input.finish[i] <= input.start[j] || input.finish[j] <= input.start[i])) valid = false;
        }
        if (valid && count > best) best = count;
    }
    return best;
}
inline void checkActivityCase(const algogenie::ActivityCase& input, bool replayTest) {
    using namespace algogenie;
    ActivityOrder order(input);
    int expected = bruteActivity(input);
    check(activityOptimum(input) == expected, "prefix verifier agrees with subset oracle");
    for (int algorithm = 0; algorithm < 2; ++algorithm) {
        ActivityTrace trace; ActivityMetrics metrics;
        ActivityResult result = algorithm ? activityDP(input, order, &metrics, &trace) : activityGreedy(input, order, &metrics, &trace);
        check(activityVerified(input, result) && result.count == expected, "activity solver reaches brute-force optimum");
        long long checksExpected = algorithm ? input.size() * (input.size() - 1LL) / 2 : (input.size() ? input.size() - 1 : 0);
        check(metrics.compatibilityChecks == checksExpected, "activity compatibility counting rule");
        if (!replayTest) continue;
        ActivityReplay replay(input.size(), trace);
        for (int k = 1; k <= trace.size(); ++k) {
            replay.next();
            IntArray best(replay.best), previous(replay.predecessor), selected(replay.selected);
            replay.back(); replay.next();
            check(equalArrays(best, replay.best) && equalArrays(previous, replay.predecessor) && equalArrays(selected, replay.selected),
                "activity back/next restores DP and selection state");
        }
        int count = 0; bool agrees = true;
        for (int i = 0; i < input.size(); ++i) if (replay.selected[i]) {
            agrees = agrees && count < result.count && order.ids[i] == result.selected[count]; ++count;
        }
        check(replay.complete() && agrees && count == result.count, "activity final replay equals actual selected IDs");
        replay.next(); check(replay.position() == trace.size(), "activity next clamps");
        replay.restart(); replay.back();
        check(replay.position() == 0 && !replay.current(), "activity restart and back clamp");
    }
}
inline void activityChecks() {
    using namespace algogenie;
    // All ordered inputs over four intervals, including duplicate identities.
    const int starts[] = {0, 1, 0, -2}, finishes[] = {1, 2, 3, 0};
    for (int n = 0; n <= 5; ++n) {
        int combinations = 1; for (int i = 0; i < n; ++i) combinations *= 4;
        for (int code = 0; code < combinations; ++code) {
            ActivityCase input(n); int digits = code;
            for (int i = 0; i < n; ++i) { int v = digits % 4; digits /= 4; input.start[i] = starts[v]; input.finish[i] = finishes[v]; }
            checkActivityCase(input, n <= 3);
        }
    }
    ActivityCase extremes(2); extremes.start[0] = INT_MIN; extremes.finish[0] = 0;
    extremes.start[1] = 0; extremes.finish[1] = INT_MAX;
    checkActivityCase(extremes, true);
    ActivityCase lesson(4); int s[] = {1,2,3,4}, f[] = {3,5,4,6};
    for (int i = 0; i < 4; ++i) { lesson.start[i] = s[i]; lesson.finish[i] = f[i]; }
    ActivityOrder order(lesson); ActivityTrace tiny(1); ActivityMetrics metrics;
    auto greedy = activityGreedy(lesson, order, &metrics, &tiny);
    check(greedy.count == 3 && greedy.selected[0] == 0 && greedy.selected[1] == 2 && greedy.selected[2] == 3, "proposal lesson IDs 1 3 4");
    ActivityReplay partial(4, tiny); partial.seek(tiny.size());
    check(tiny.truncated() && !partial.complete(), "activity truncated history never claims completion");
    ActivityCase empty; ActivityOrder emptyOrder(empty); activityDP(empty, emptyOrder, &metrics, &tiny);
    check(!tiny.truncated() && metrics.compatibilityChecks == 0 && tiny.size() == 1, "activity observer reuse clears old data");
    ActivityResult bad(greedy); bad.selected[1] = bad.selected[0];
    check(!activityVerified(lesson, bad), "duplicate selected ID rejected");
    bad = greedy; bad.count = 1; check(!activityVerified(lesson, bad), "feasible suboptimal result rejected");
    lesson.finish[0] = lesson.start[0]; check(!lesson.valid(), "zero-length activity rejected");
    ActivityCase large(100); for (int i = 0; i < 100; ++i) { large.start[i] = i; large.finish[i] = i+1; }
    ActivityOrder largeOrder(large);
    check(activityGreedy(large, largeOrder).count == 100 && activityDP(large, largeOrder).count == 100, "100 touching activities");
}
