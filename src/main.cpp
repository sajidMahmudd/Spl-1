#include <iostream>
#include "algorithms/sorting/MergeSort.h"
#include "verify/Sorting.h"
#include "io/Input.h"

void printArray(const IntArray& values) {
    std::cout << "[ ";
    for (int i = 0; i < values.size(); ++i) std::cout << values[i] << ' ';
    std::cout << "]\n";
}

bool readNumber(const char* prompt, int minimum, int maximum, int& result) {
    char line[128]; bool tooLong;
    while (true) {
        std::cout << prompt << std::flush;
        if (!readLine(std::cin, line, 128, tooLong)) return false;
        IntArray number(1);
        if (!tooLong && parseIntegers(line, number) && number[0] >= minimum && number[0] <= maximum) {
            result = number[0]; return true;
        }
        std::cout << "  Please enter one whole number from " << minimum << " to " << maximum << ".\n";
    }
}

int main() {
    std::cout << "\n============================================================\n"
              << "                         ALGOGENIE\n"
              << "          See how an algorithm makes its decisions.\n"
              << "============================================================\n"
              << "  FIRST MILESTONE | Merge Sort + verified results\n"
              << "  Replay and comparison are coming in the next milestones.\n";
    while (true) {
        std::cout << "\n  1  Sort your array\n  2  Run example: 8 3 5 3 1\n  0  Exit\n\n";
        int choice;
        if (!readNumber("  Choose > ", 0, 2, choice) || choice == 0) break;
        int count = 5;
        if (choice == 1 && !readNumber("  Number of elements (0-100) > ", 0, 100, count)) break;
        IntArray original(count);
        if (choice == 2) {
            const int example[] = {8, 3, 5, 3, 1};
            for (int i = 0; i < count; ++i) original[i] = example[i];
        } else if (count > 0) {
            char line[4096]; bool tooLong;
            while (true) {
                std::cout << "  Enter exactly " << count << " integers on one line > " << std::flush;
                if (!readLine(std::cin, line, 4096, tooLong)) return 0;
                if (!tooLong && parseIntegers(line, original)) break;
                std::cout << "  Invalid input: use exactly " << count << " integers in the int range.\n";
            }
        }
        IntArray result(original);
        algogenie::mergeSort(result);
        const bool ordered = isSorted(result);
        const bool preserved = preservesValues(original, result);
        std::cout << "\n------------------------------------------------------------\n"
                  << "  MERGE SORT | RESULT\n\n  Input   "; printArray(original);
        std::cout << "  Output  "; printArray(result);
        std::cout << "\n  Sorted order     : " << (ordered ? "PASS" : "FAIL")
                  << "\n  Values preserved : " << (preserved ? "PASS" : "FAIL")
                  << "\n\n  Method: split into halves, sort each half, then merge.\n"
                  << "  Solver: O(n log n) time, O(n) auxiliary storage.\n"
                  << "  Verification is separate and uses O(n^2) frequency checks.\n"
                  << "------------------------------------------------------------\n";
        if (!ordered || !preserved) return 1;
    }
    std::cout << "\n  Session finished.\n";
    return 0;
}
