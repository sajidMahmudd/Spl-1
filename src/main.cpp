#include "algorithms/sorting/MergeSort.h"
#include "ui/MergeViewer.h"

int main() {
    std::cout << "\n====================================================================\n"
              << "                           ALGOGENIE\n"
              << "            See how an algorithm makes its decisions.\n"
              << "====================================================================\n"
              << "  WORKING: Merge Sort, learning replay, result verification\n"
              << "  NEXT: Quicksort and comparison, followed by saved cases/reports\n";
    while (true) {
        std::cout << "\n  1  LEARN / Merge Sort step by step\n"
                  << "  2  RUN / Merge Sort result only\n  0  Exit\n\n";
        int choice;
        if (!readNumber("  Choose > ", 0, 2, choice) || choice == 0) break;
        IntArray original;
        if (!readCase(original, choice == 1 ? 30 : 100)) break;
        IntArray result(original);
        SortTrace trace;
        algogenie::mergeSort(result, choice == 1 ? &trace : nullptr);
        if (!isSorted(result) || !preservesValues(original, result)) {
            std::cout << "  INTERNAL CHECK FAILED. Result rejected.\n";
            return 1;
        }
        if (choice == 1) {
            std::cout << "\n  The solver has recorded its decisions. Replay now starts at your input.\n";
            learnMerge(original, trace);
        } else {
            std::cout << "\n  MERGE SORT / VERIFIED RESULT\n  Input   "; printArray(original);
            std::cout << "  Output  "; printArray(result);
            std::cout << "\n  Sorted order     : PASS\n  Values preserved : PASS\n"
                      << "  Solver: O(n log n) time, O(n) auxiliary storage.\n"
                      << "  Separate verification: O(n^2) frequency checks.\n";
        }
    }
    std::cout << "\n  Session finished.\n";
    return 0;
}
