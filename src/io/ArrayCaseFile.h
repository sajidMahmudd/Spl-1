#pragma once
#include <fstream>
#include <cstring>
#include "Input.h"

namespace algogenie {
inline bool tokenSpace(char c) { return space(c) || c == '\n' || c == '\v' || c == '\f'; }
// Bounded handwritten token reader; overlong tokens are consumed and rejected.
inline bool readToken(std::istream& in, char* token, int capacity, bool& overflow) {
    int used = 0; char c; overflow = false;
    while (in.get(c)) {
        if (!tokenSpace(c)) {
            do {
                if (c == '\0') overflow = true; // Never accept a hidden token suffix after NUL.
                if (used < capacity - 1) token[used++] = c;
                else overflow = true;
            } while (in.get(c) && !tokenSpace(c));
            token[used] = '\0'; return true;
        }
    }
    token[0] = '\0'; return false;
}
inline bool expectToken(std::istream& in, const char* expected) {
    char token[64]; bool overflow;
    return readToken(in, token, 64, overflow) && !overflow && std::strcmp(token, expected) == 0;
}
inline bool readIntegerToken(std::istream& in, int& value) {
    char token[64]; bool overflow; IntArray number(1);
    if (!readToken(in, token, 64, overflow) || overflow || !parseIntegers(token, number)) return false;
    value = number[0]; return true;
}
inline bool readArrayCase(std::istream& in, IntArray& destination, const char*& error) {
    error = nullptr;
    // Accept the optional UTF-8 BOM used by some text editors.
    if (in.peek() == 0xEF) {
        in.get();
        if (in.get() != 0xBB || in.get() != 0xBF) { error = "Invalid UTF-8 file header."; return false; }
    }
    if (!expectToken(in, "ALGOGENIE") || !expectToken(in, "1")) {
        error = "Expected ALGOGENIE 1 (supported format version)."; return false;
    }
    if (!expectToken(in, "TYPE") || !expectToken(in, "ARRAY") || !expectToken(in, "COUNT")) {
        error = "Expected TYPE ARRAY followed by COUNT."; return false;
    }
    int count;
    if (!readIntegerToken(in, count) || count < 0 || count > 100) {
        error = "COUNT must be a whole number from 0 to 100."; return false;
    }
    if (!expectToken(in, "VALUES")) { error = "Missing VALUES field."; return false; }
    IntArray candidate(count);
    for (int i = 0; i < count; ++i) {
        if (!readIntegerToken(in, candidate[i])) {
            error = "Missing value, malformed integer, or integer overflow."; return false;
        }
    }
    if (!expectToken(in, "END")) { error = "Expected END after exactly COUNT values."; return false; }
    char extra[64]; bool overflow;
    if (readToken(in, extra, 64, overflow)) { error = "Unexpected data after END."; return false; }
    if (in.bad() || !in.eof()) { error = "Could not finish reading the file."; return false; }
    destination = candidate; // Commit only after the entire file has passed validation.
    return true;
}
inline bool writeArrayCase(std::ostream& out, const IntArray& input) {
    out << "ALGOGENIE 1\nTYPE ARRAY\nCOUNT " << input.size() << "\nVALUES";
    for (int i = 0; i < input.size(); ++i) out << ' ' << input[i];
    out << "\nEND\n";
    return static_cast<bool>(out);
}
inline bool loadArrayCase(const char* path, IntArray& destination, const char*& error) {
    std::ifstream in(path, std::ios::binary);
    if (!in) { error = "Cannot open that file. Check its path and permissions."; return false; }
    return readArrayCase(in, destination, error);
}
inline bool fileExists(const char* path) { std::ifstream in(path, std::ios::binary); return in.good(); }
inline bool saveArrayCase(const char* path, const IntArray& input, const char*& error) {
    error = nullptr;
    if (input.size() > 100) { error = "Case exceeds the supported 100-element limit."; return false; }
    if (fileExists(path)) { error = "File already exists. Choose a new filename."; return false; }
    std::ofstream out(path);
    if (!out) { error = "Cannot create the file. The folder must exist and be writable."; return false; }
    const bool written = writeArrayCase(out, input);
    out.close();
    if (!written || out.fail()) { error = "File could not be fully written."; return false; }
    return true;
}
}
