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
inline bool readCase(IntArray& original, int maximum, const IntArray* current = nullptr) {
    std::cout << "\n  CHOOSE INPUT\n  1  Your own array\n  2  Small lesson: 4 1 3\n"
              << "  3  Duplicate example: 8 3 5 3 1\n  4  Sorted: 1 2 3 4 5 6 7 8\n"
              << "  5  Reversed: 8 7 6 5 4 3 2 1\n  6  Equal: 5 5 5 5 5 5 5 5\n"
              << "  7  Use current / loaded case\n";
    int choice, count;
    while (true) {
        if (!readNumber("  Choose > ", 1, 7, choice)) return false;
        if (choice != 7) break;
        if (!current) { std::cout << "  No current case. Choose another input or load one using main menu 4.\n"; continue; }
        if (current->size() > maximum) {
            std::cout << "  This mode accepts at most " << maximum << " elements. Choose a smaller input.\n"; continue;
        }
        original = *current; return true;
    }
    if (choice != 1) {
        const int small[] = {4, 1, 3}, duplicates[] = {8, 3, 5, 3, 1};
        count = choice == 2 ? 3 : choice == 3 ? 5 : 8; original = IntArray(count);
        for (int i = 0; i < count; ++i) {
            if (choice == 2) original[i] = small[i];
            else if (choice == 3) original[i] = duplicates[i];
            else if (choice == 4) original[i] = i + 1;
            else if (choice == 5) original[i] = 8 - i;
            else original[i] = 5;
        }
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
