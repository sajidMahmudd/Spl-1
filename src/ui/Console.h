#pragma once
#include <iostream>
#include "../io/Input.h"

inline void printArray(const IntArray& values) {
    std::cout << "[ ";
    for (int i = 0; i < values.size(); ++i) std::cout << values[i] << ' ';
    std::cout << "]\n";
}
inline bool readNumber(const char* prompt, int minimum, int maximum, int& result) {
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
inline bool readCase(IntArray& original, int maximum) {
    std::cout << "\n  CHOOSE INPUT\n  1  Your own array\n  2  Small lesson: 4 1 3\n  3  Duplicate example: 8 3 5 3 1\n";
    int choice, count;
    if (!readNumber("  Choose > ", 1, 3, choice)) return false;
    if (choice != 1) {
        const int small[] = {4, 1, 3}, duplicates[] = {8, 3, 5, 3, 1};
        count = choice == 2 ? 3 : 5; original = IntArray(count);
        for (int i = 0; i < count; ++i) original[i] = choice == 2 ? small[i] : duplicates[i];
        return true;
    }
    std::cout << "  Supported size: 0 to " << maximum << " elements.\n";
    if (!readNumber("  Number of elements > ", 0, maximum, count)) return false;
    original = IntArray(count);
    if (count == 0) return true;
    char line[4096]; bool tooLong;
    while (true) {
        std::cout << "  Enter exactly " << count << " integers on one line > " << std::flush;
        if (!readLine(std::cin, line, 4096, tooLong)) return false;
        if (!tooLong && parseIntegers(line, original)) return true;
        std::cout << "  Invalid input: use exactly " << count << " integers in the int range.\n";
    }
}
