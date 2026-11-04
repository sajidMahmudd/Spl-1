#include <iostream>
#include <climits>
#include "../src/algorithms/sorting/MergeSort.h"
#include "../src/verify/Sorting.h"

int checks = 0, failures = 0;
void check(bool condition, const char* name) {
    ++checks;
    if (!condition) { ++failures; std::cout << "FAIL: " << name << '\n'; }
}
void testSort(const int* input, const int* expected, int size) {
    IntArray values(size);
    for (int i = 0; i < size; ++i) values[i] = input[i];
    IntArray original(values);
    algogenie::mergeSort(values);
    check(isSorted(values), "sorted order");
    check(preservesValues(original, values), "value preservation");
    bool matches = true;
    for (int i = 0; i < size; ++i) if (values[i] != expected[i]) matches = false;
    check(matches, "known expected output");
}
int main() {
    testSort(nullptr, nullptr, 0);
    const int single[] = {7}; testSort(single, single, 1);
    const int mixed[] = {5, 1, 5, -2, 0}, sorted[] = {-2, 0, 1, 5, 5};
    testSort(mixed, sorted, 5); testSort(sorted, sorted, 5);
    const int reverse[] = {5, 5, 1, 0, -2}; testSort(reverse, sorted, 5);
    const int equal[] = {3, 3, 3}; testSort(equal, equal, 3);
    const int extremes[] = {INT_MAX, 0, INT_MIN}, extremesSorted[] = {INT_MIN, 0, INT_MAX};
    testSort(extremes, extremesSorted, 3);
    IntArray a(2); a[0] = 1; a[1] = 2;
    IntArray b(a); b[0] = 2;
    check(a[0] == 1, "deep copy independence");
    check(!preservesValues(a, b), "checker rejects changed multiplicity");
    b[0] = 2; b[1] = 1;
    check(!isSorted(b), "checker rejects disorder");
    IntArray c; c = a; c[0] = 9;
    check(a[0] == 1, "assignment independence");
    c = c; check(c[0] == 9, "self assignment");
    // Exhaust all 3^n arrays over {-1,0,1}, n <= 7.
    for (int n = 0; n <= 7; ++n) {
        int combinations = 1;
        for (int i = 0; i < n; ++i) combinations *= 3;
        for (int code = 0; code < combinations; ++code) {
            IntArray values(n); int digits = code;
            for (int i = 0; i < n; ++i) { values[i] = digits % 3 - 1; digits /= 3; }
            IntArray original(values); algogenie::mergeSort(values);
            check(isSorted(values) && preservesValues(original, values), "exhaustive tiny input");
        }
    }
    std::cout << checks - failures << '/' << checks << " checks passed\n";
    return failures ? 1 : 0;
}
