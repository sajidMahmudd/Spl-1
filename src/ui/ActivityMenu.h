#pragma once
#include "FileMenu.h"
#include "ActivityViewer.h"
#include "../io/ActivityFile.h"

namespace algogenie {
inline bool readActivities(ActivityCase& input, int limit, const ActivityCase* current) {
    std::cout << "\n  ACTIVITY INPUT / maximum " << limit << " activities\n"
        << "  1  Custom intervals\n  2  Proposal lesson: (1,3) (2,5) (3,4) (4,6)\n"
        << "  3  All overlap\n  4  Touching + negative times\n  5  Equal finish / duplicate intervals\n"
        << "  6  Use current / loaded activity case\n";
    int choice;
    while (true) {
        if (!readNumber("  Choose > ",1,6,choice)) return false;
        if (choice != 6) break;
        if (!current) { std::cout << "  No current activity case; choose another input or load a case.\n"; continue; }
        if (current->size() > limit) { std::cout << "  This mode accepts at most " << limit << " activities. Choose a smaller input.\n"; continue; }
        input = *current; return true;
    }
    if (choice != 1) {
        ActivityCase candidate(4);
        const int starts[4][4] = {{1,2,3,4},{0,1,2,3},{-4,-2,0,2},{0,1,0,3}};
        const int finishes[4][4] = {{3,5,4,6},{10,9,8,7},{-2,0,2,4},{3,3,3,5}};
        for (int i = 0; i < 4; ++i) { candidate.start[i] = starts[choice-2][i]; candidate.finish[i] = finishes[choice-2][i]; }
        input = candidate; return true;
    }
    int n;
    if (!readNumber("  Number of activities > ",0,limit,n)) return false;
    ActivityCase candidate(n);
    std::cout << "  Enter START FINISH for each row. Signed int times; start < finish.\n"
        << "  IDs are original row numbers. Equal endpoints between activities may touch.\n";
    for (int i = 0; i < n; ++i) {
        while (true) {
            std::cout << "  ID " << i+1 << " > " << std::flush;
            char line[256]; bool tooLong; IntArray pair(2);
            if (!readLine(std::cin,line,256,tooLong)) return false;
            if (!tooLong && parseIntegers(line,pair) && pair[0] < pair[1]) {
                candidate.start[i] = pair[0]; candidate.finish[i] = pair[1]; break;
            }
            std::cout << "  Invalid interval: enter exactly two integers with start < finish.\n";
        }
    }
    input = candidate; return true;
}
inline void activityCaseMenu(ActivityCase& current, bool& haveCurrent, ActivityReport& latest) {
    std::cout << "\n  ACTIVITY CASE FILES\n  1  Save current activity input\n  2  Load activity input\n  0  Back\n";
    int choice;
    if (!readNumber("  Choose > ",0,2,choice) || choice == 0) return;
    if (choice == 1 && !haveCurrent) { std::cout << "  No current activity input.\n"; return; }
    char path[512]; const char* error = nullptr;
    if (!readFilePath(choice == 1 ? "  Save activity .txt path" : "  Load activity .txt path",path,512,".txt")) return;
    if (choice == 1) {
        if (saveActivityCase(path,current,error)) std::cout << "  SAVED ACTIVITY CASE: " << path << '\n';
        else std::cout << "  Save failed: " << error << '\n';
    } else {
        ActivityCase loaded;
        if (loadActivityCase(path,loaded,error)) {
            current = loaded; haveCurrent = true; latest = ActivityReport{};
            std::cout << "  LOADED ACTIVITY CASE: " << path << "\n  Use input option 6. Previous activity report cleared.\n";
        } else std::cout << "  Load failed: " << error << " Current activity input/report unchanged.\n";
    }
}
inline void activityExportMenu(const ActivityReport& latest) {
    if (!latest.valid()) { std::cout << "  No verified activity report available. Run a mode first.\n"; return; }
    std::cout << "\n  EXPORT ACTIVITY REPORT\n  1  TXT\n  2  CSV\n  0  Back\n";
    int choice;
    if (!readNumber("  Choose > ",0,2,choice) || choice == 0) return;
    char path[512]; const char* error = nullptr;
    if (!readFilePath("  Activity report path",path,512,choice == 1 ? ".txt" : ".csv")) return;
    if (exportActivityReport(path,latest,choice == 2,error)) std::cout << "  EXPORTED ACTIVITY REPORT: " << path << '\n';
    else std::cout << "  Export failed: " << error << '\n';
}
inline void activityMenu(ActivityCase& current, bool& haveCurrent, ActivityReport& latest) {
    while (true) {
        std::cout << "\n====================================================================\n"
            << "  ALGOGENIE / ACTIVITY SELECTION\n  Maximize compatible activity COUNT (unweighted).\n"
            << "  1  LEARN / Greedy or DP\n  2  RUN / Selected activities\n  3  COMPARE / Greedy vs DP\n"
            << "  4  CASE FILES / Activity TXT\n  5  EXPORT / Activity TXT or CSV\n  0  Main menu\n";
        if (haveCurrent) std::cout << "  Current activity case: " << current.size() << " activities (input option 6).\n";
        int mode;
        if (!readNumber("  Choose > ",0,5,mode) || mode == 0) return;
        if (mode == 4) { activityCaseMenu(current,haveCurrent,latest); continue; }
        if (mode == 5) { activityExportMenu(latest); continue; }
        int algorithm = 1;
        if (mode != 3) {
            std::cout << "  1  Earliest-finish Greedy\n  2  DP / Best chain ending at each activity\n";
            if (!readNumber("  Algorithm > ",1,2,algorithm)) return;
        }
        ActivityCase input;
        if (!readActivities(input,mode == 1 ? activityLearnLimit : activityLimit,haveCurrent ? &current : nullptr)) return;
        ActivityTrace trace;
        ActivityReport report = mode == 3 ? compareActivities(input) : runActivity(input,algorithm == 2,mode == 1 ? &trace : nullptr);
        if (!report.valid()) { std::cout << "  INTERNAL CHECK FAILED. Activity result rejected; prior state unchanged.\n"; continue; }
        current = input; haveCurrent = true; latest = report;
        if (mode == 1) learnActivity(input,trace,algorithm == 2);
        else showActivityReport(report);
    }
}
}
