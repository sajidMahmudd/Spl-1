#pragma once
#include <cstring>
#include "Console.h"
#include "../io/ReportFile.h"

inline bool readFilePath(const char* prompt, char* path, int capacity, const char* extension) {
    while (true) {
        std::cout << prompt << " (blank = cancel) > " << std::flush;
        bool tooLong;
        if (!readLine(std::cin, path, capacity, tooLong)) return false;
        if (tooLong) { std::cout << "  Path is too long.\n"; continue; }
        char* start = path; while (space(*start)) ++start;
        int length = static_cast<int>(std::strlen(start));
        while (length > 0 && space(start[length - 1])) --length;
        if (length >= 2 && start[0] == '"' && start[length - 1] == '"') { ++start; length -= 2; }
        if (length == 0) return false;
        for (int i = 0; i < length; ++i) path[i] = start[i];
        path[length] = '\0';
        bool matches = length >= 4;
        for (int i = 0; matches && i < 4; ++i) {
            char c = path[length - 4 + i];
            if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
            matches = c == extension[i];
        }
        if (matches) return true;
        std::cout << "  Please use a filename ending in " << extension << ".\n";
    }
}
inline void caseMenu(IntArray& current, bool& haveCurrent, algogenie::RunReport& latest) {
    std::cout << "\n  CASE FILES\n  1  Save current input as TXT\n  2  Load input from TXT\n  0  Back\n";
    int choice;
    if (!readNumber("  Choose > ", 0, 2, choice) || choice == 0) return;
    if (choice == 1 && !haveCurrent) {
        std::cout << "  No current input. Run a lesson/comparison or load a case first.\n"; return;
    }
    char path[512]; const char* error = nullptr;
    if (!readFilePath(choice == 1 ? "  Save path, e.g. saved/my-case.txt" : "  Load path, e.g. examples/sorted.txt", path, 512, ".txt")) return;
    if (choice == 1) {
        if (algogenie::saveArrayCase(path, current, error)) std::cout << "  SAVED CASE: " << path << '\n';
        else std::cout << "  Save failed: " << error << '\n';
    } else {
        IntArray loaded;
        if (algogenie::loadArrayCase(path, loaded, error)) {
            current = loaded; haveCurrent = true; latest = algogenie::RunReport{};
            std::cout << "  LOADED CASE: " << path << " (" << current.size() << " elements)\n"
                      << "  Choose Learn, Run or Compare, then input 7 to use this case.\n"
                      << "  The previous report was cleared; run this input to create a new report.\n";
        } else std::cout << "  Load failed: " << error << " Current input/report unchanged.\n";
    }
}
inline void exportMenu(const algogenie::RunReport& latest) {
    if (!latest.valid()) {
        std::cout << "  No verified report available. Run Learn, Run or Compare first.\n"; return;
    }
    std::cout << "\n  EXPORT LATEST " << algogenie::reportMode(latest) << " REPORT\n"
              << "  1  Readable TXT report\n  2  CSV report\n  0  Back\n";
    int choice;
    if (!readNumber("  Choose > ", 0, 2, choice) || choice == 0) return;
    char path[512]; const char* error = nullptr;
    if (!readFilePath(choice == 1 ? "  TXT path, e.g. reports/my-report.txt" : "  CSV path, e.g. reports/my-report.csv",
        path, 512, choice == 1 ? ".txt" : ".csv")) return;
    if (algogenie::exportReport(path, latest, choice == 2, error)) std::cout << "  EXPORTED REPORT: " << path << '\n';
    else std::cout << "  Export failed: " << error << '\n';
}
