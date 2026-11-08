#pragma once
#include <iomanip>
#include <cstdlib>
#include "Console.h"
#include "../app/SortComparison.h"

template <typename T>
inline void comparisonRow(const char* label, T merge, T quick) {
    std::cout << "  " << std::left << std::setw(27) << label << std::right
              << std::setw(16) << merge << std::setw(16) << quick << '\n';
}
inline bool showComparison(const IntArray& original, const algogenie::SortComparison& report) {
    std::cout << "\n====================================================================\n"
              << "  ALGOGENIE / COMPARE / SORTING\n"
              << "  Same input. Independent working copies. Checked results.\n"
              << "--------------------------------------------------------------------\n"
              << "  Input   "; printArray(original);
    std::cout << "  Merge   "; printArray(report.merge.result);
    std::cout << "  Quick   "; printArray(report.quick.result);
    std::cout << '\n';
    comparisonRow("CHECK / METRIC", "MERGE SORT", "QUICKSORT");
    comparisonRow("Sorted order", report.merge.ordered ? "PASS" : "FAIL", report.quick.ordered ? "PASS" : "FAIL");
    comparisonRow("Values preserved", report.merge.preserved ? "PASS" : "FAIL", report.quick.preserved ? "PASS" : "FAIL");
    comparisonRow("Value comparisons", report.merge.metrics.comparisons, report.quick.metrics.comparisons);
    comparisonRow("Main-array writes", report.merge.metrics.mainWrites, report.quick.metrics.mainWrites);
    comparisonRow("Merge-buffer writes", report.merge.metrics.bufferWrites, report.quick.metrics.bufferWrites);
    comparisonRow("Swaps (distinct indices)", report.merge.metrics.swaps, report.quick.metrics.swaps);
    comparisonRow("Max recursion depth", report.merge.metrics.maximumDepth, report.quick.metrics.maximumDepth);
    std::cout << "\n  Matching sorted outputs: " << (report.equalOutputs ? "PASS" : "FAIL") << '\n';
    if (!report.valid()) {
        std::cout << "  CHECK FAILED. This comparison is not accepted; timing is withheld.\n";
        return false;
    }
    std::cout << "\n  WHAT THE COUNTS MEAN\n"
              << "  Comparisons: values only; loop and index checks are excluded.\n"
              << "  Writes: solver assignments only; input copies/zero-fill excluded.\n"
              << "  A distinct-index swap = 2 main writes. Self-swaps count as 0.\n"
              << "  Depth: root is 0, including base-case calls.\n";
    const auto flags = std::cout.flags(); const auto precision = std::cout.precision();
    std::cout << "\n  SEPARATE TIMING / MICROSECONDS\n" << std::fixed << std::setprecision(3);
    std::cout << "  " << std::left << std::setw(27) << "Median of 7 runs (us)" << std::right;
    if (report.merge.medianUs == 0) std::cout << std::setw(16) << "not resolved";
    else std::cout << std::setw(16) << report.merge.medianUs;
    if (report.quick.medianUs == 0) std::cout << std::setw(16) << "not resolved";
    else std::cout << std::setw(16) << report.quick.medianUs;
    std::cout << '\n';
    std::cout.flags(flags); std::cout.precision(precision);
    std::cout << "  1 warm-up each; 7 fresh-input runs each; alternating run order.\n"
              << "  No trace/count collection, input copying, display or checks timed.\n"
              << "  Solver-internal allocation is included (Merge Sort's buffer).\n"
              << "  All warm-up and timed results matched checked outputs.\n";
#ifdef ALGOGENIE_RELEASE_BUILD
    std::cout << "  Build: Release (-O2), GCC " << __VERSION__ << ", C++17.\n";
#else
    std::cout << "  Build: Debug (-O0), GCC " << __VERSION__ << ", C++17.\n"
              << "  Debug timings are illustrative; use a Release build for experiments.\n";
#endif
    const char* machine = std::getenv("COMPUTERNAME");
    if (machine) std::cout << "  Machine: " << machine << '\n';
    std::cout << "  Not resolved = median below measurable clock resolution.\n"
              << "  Tiny inputs are noisy; displayed precision is not accuracy.\n"
              << "  No overall speed winner is inferred from this small experiment.\n";
    if (original.size() > 1 && report.quick.metrics.maximumDepth == original.size() - 1) {
        std::cout << "\n  LESSON: Quicksort repeatedly produced an empty side here.\n"
                  << "  Last-element pivots can give quadratic work on this input.\n";
    }
    std::cout << "====================================================================\n";
    return true;
}
