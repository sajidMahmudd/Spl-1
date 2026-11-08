#include "algorithms/sorting/MergeSort.h"
#include "algorithms/sorting/QuickSort.h"
#include "ui/SortViewer.h"
#include "ui/ComparisonView.h"
#include "ui/FileMenu.h"

int main() {
    std::cout << "\n====================================================================\n"
              << "                           ALGOGENIE\n"
              << "            See how an algorithm makes its decisions.\n"
              << "====================================================================\n"
              << "  SORTING LAB / Merge Sort + Quicksort\n"
              << "  Learn | Compare | Verify | Save/load | TXT/CSV reports\n";
    IntArray current;
    bool haveCurrent = false;
    algogenie::RunReport latest;
    while (true) {
        std::cout << "\n  1  LEARN / Sorting step by step\n"
                  << "  2  RUN / Sorted result only\n"
                  << "  3  COMPARE / Merge Sort vs Quicksort\n"
                  << "  4  CASE FILES / Save or load TXT\n"
                  << "  5  EXPORT / Latest TXT or CSV report\n  0  Exit\n\n";
        if (haveCurrent) std::cout << "  Current case: " << current.size() << " elements (input option 7).\n";
        int choice;
        if (!readNumber("  Choose > ", 0, 5, choice) || choice == 0) break;
        if (choice == 4) { caseMenu(current, haveCurrent, latest); continue; }
        if (choice == 5) { exportMenu(latest); continue; }
        if (choice == 3) {
            IntArray original;
            if (!readCase(original, 100, haveCurrent ? &current : nullptr)) break;
            const auto comparison = algogenie::compareSorts(original);
            if (!showComparison(original, comparison)) return 1;
            current = original; haveCurrent = true;
            latest = algogenie::RunReport{};
            latest.available = true; latest.comparison = true; latest.input = original;
            latest.first = comparison.merge; latest.second = comparison.quick;
            continue;
        }
        std::cout << "\n  CHOOSE ALGORITHM\n  1  Merge Sort\n  2  Quicksort / Lomuto / last-element pivot\n";
        int algorithm;
        if (!readNumber("  Choose > ", 1, 2, algorithm)) break;
        const bool quick = algorithm == 2;
        IntArray original;
        if (!readCase(original, choice == 1 ? 30 : 100, haveCurrent ? &current : nullptr)) break;
        IntArray result(original);
        SortTrace trace;
        SortMetrics metrics;
        if (quick) algogenie::quickSort(result, choice == 1 ? &trace : nullptr, &metrics);
        else algogenie::mergeSort(result, choice == 1 ? &trace : nullptr, &metrics);
        if (!isSorted(result) || !preservesValues(original, result)) {
            std::cout << "  INTERNAL CHECK FAILED. Result rejected.\n";
            return 1;
        }
        current = original; haveCurrent = true;
        latest = algogenie::RunReport{};
        latest.available = true; latest.learning = choice == 1; latest.quick = quick;
        latest.input = original; latest.first.result = result; latest.first.metrics = metrics;
        latest.first.ordered = true; latest.first.preserved = true;
        latest.traceEvents = trace.size(); latest.traceTruncated = trace.truncated();
        if (choice == 1) {
            std::cout << "\n  The solver has recorded its decisions. Replay now starts at your input.\n";
            learnSort(original, trace, quick);
        } else {
            std::cout << "\n  " << (quick ? "QUICKSORT" : "MERGE SORT") << " / VERIFIED RESULT\n  Input   "; printArray(original);
            std::cout << "  Output  "; printArray(result);
            std::cout << "\n  Sorted order     : PASS\n  Values preserved : PASS\n";
            if (quick) std::cout << "  Last-element Lomuto: average O(n log n), worst O(n^2).\n"
                                 << "  Partition workspace O(1); recursion stack worst O(n).\n";
            else std::cout << "  Solver: O(n log n) time, O(n) auxiliary storage.\n";
            std::cout << "  Separate verification: O(n^2) frequency checks.\n";
        }
    }
    std::cout << "\n  Session finished.\n";
    return 0;
}
