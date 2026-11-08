#pragma once
#include "../app/SortComparison.h"

namespace algogenie {
struct RunReport {
    RunReport() {} // Default-initialize owned arrays without aggregate conversion warnings.
    bool available = false;
    bool comparison = false, learning = false, quick = false;
    IntArray input;
    SortMeasurement first, second;
    int traceEvents = 0;
    bool traceTruncated = false;
    bool valid() const {
        if (!available || !first.ordered || !first.preserved || !isSorted(first.result) ||
            !preservesValues(input, first.result)) return false;
        if (comparison) return first.timedResultsValid && second.timedResultsValid &&
            second.ordered && second.preserved && isSorted(second.result) &&
            preservesValues(input, second.result) && sameArray(first.result, second.result);
        return true;
    }
};
inline const char* reportMode(const RunReport& report) {
    return report.comparison ? "compare" : report.learning ? "learn" : "run";
}
inline const char* buildLabel() {
#ifdef ALGOGENIE_RELEASE_BUILD
    return "Release -O2";
#else
    return "Debug -O0 -g";
#endif
}
inline const char* buildId() { return "sorting-v0.5 " __DATE__ " " __TIME__; }
}
