#pragma once
#include <istream>
#include <climits>
#include "../core/IntArray.h"

inline bool space(char c) { return c == ' ' || c == '\t' || c == '\r'; }
inline bool parseIntegers(const char* text, IntArray& output) {
    int count = 0;
    const char* p = text;
    while (*p) {
        while (space(*p)) ++p;
        if (!*p) break;
        if (count >= output.size()) return false;
        bool negative = false;
        if (*p == '-' || *p == '+') { negative = *p == '-'; ++p; }
        if (*p < '0' || *p > '9') return false;
        const unsigned long long limit = negative ? static_cast<unsigned long long>(INT_MAX) + 1 : INT_MAX;
        unsigned long long value = 0;
        while (*p >= '0' && *p <= '9') {
            unsigned digit = static_cast<unsigned>(*p - '0');
            if (value > (limit - digit) / 10) return false;
            value = value * 10 + digit; ++p;
        }
        if (*p && !space(*p)) return false;
        output[count++] = negative ? static_cast<int>(-static_cast<long long>(value)) : static_cast<int>(value);
    }
    return count == output.size();
}

// Consume the entire line even if it exceeds the buffer, so retry starts cleanly.
inline bool readLine(std::istream& in, char* buffer, int capacity, bool& tooLong) {
    int used = 0; char c; bool received = false; tooLong = false;
    while (in.get(c)) {
        received = true;
        if (c == '\n') break;
        if (used < capacity - 1) buffer[used++] = c;
        else tooLong = true;
    }
    buffer[used] = '\0';
    return received;
}
