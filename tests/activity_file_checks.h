#pragma once
#include "../src/io/ActivityFile.h"

inline void activityFileChecks() {
    using namespace algogenie;
    ActivityCase lesson(4); int s[] = {1,2,3,4}, f[] = {3,5,4,6};
    for (int i = 0; i < 4; ++i) { lesson.start[i] = s[i]; lesson.finish[i] = f[i]; }
    for (int n : {0,1,20,100}) {
        ActivityCase input(n), restored;
        for (int i = 0; i < n; ++i) { input.start[i] = i-50; input.finish[i] = i-49; }
        std::stringstream file; const char* error = nullptr;
        check(writeActivityCase(file,input) && readActivityCase(file,restored,error) &&
            equalArrays(input.start,restored.start) && equalArrays(input.finish,restored.finish), "activity case roundtrip retains input identities");
    }
    const char* malformed[] = {"", "ALGOGENIE 2 TYPE ACTIVITY COUNT 0 INTERVALS END",
        "ALGOGENIE 1 TYPE ARRAY COUNT 0 INTERVALS END", "ALGOGENIE 1 TYPE ACTIVITY COUNT 101 INTERVALS END",
        "ALGOGENIE 1 TYPE ACTIVITY COUNT -1 INTERVALS END", "ALGOGENIE 1 TYPE ACTIVITY COUNT 1 INTERVALS 1 1 END",
        "ALGOGENIE 1 TYPE ACTIVITY COUNT 1 INTERVALS 2 1 END", "ALGOGENIE 1 TYPE ACTIVITY COUNT 1 INTERVALS 1 END",
        "ALGOGENIE 1 TYPE ACTIVITY COUNT 1 INTERVALS 1 2147483648 END", "ALGOGENIE 1 TYPE ACTIVITY COUNT 0 INTERVALS END junk",
        "ALGOGENIE 1 TYPE ACTIVITY COUNT 1 INTERVALS 1.5 3 END", "ALGOGENIE 1 TYPE ACTIVITY COUNT 1 INTERVALS 1 3 4 END"};
    for (const char* text : malformed) {
        ActivityCase destination(lesson); std::istringstream in(text); const char* error = nullptr;
        check(!readActivityCase(in,destination,error) && error && equalArrays(destination.start,lesson.start) &&
            equalArrays(destination.finish,lesson.finish), "malformed activity case does not mutate destination");
    }
    std::istringstream bom("\xEF\xBB\xBF" "ALGOGENIE 1\r\nTYPE ACTIVITY COUNT 1 INTERVALS -2147483648 2147483647 END");
    ActivityCase restored; const char* error = nullptr;
    check(readActivityCase(bom,restored,error) && restored.start[0] == INT_MIN && restored.finish[0] == INT_MAX, "activity BOM and int extremes");
    std::string nul = "ALGOGENIE"; nul.push_back('\0'); nul += " 1 TYPE ACTIVITY COUNT 0 INTERVALS END";
    std::istringstream hidden(nul); check(!readActivityCase(hidden,restored,error), "activity embedded NUL rejected");
    std::istringstream bad("ALGOGENIE 1 TYPE ACTIVITY COUNT 0 INTERVALS END"); bad.setstate(std::ios::badbit);
    check(!readActivityCase(bad,restored,error), "activity bad stream rejected");
    ActivityCase original(lesson);
    auto compared = compareActivities(lesson);
    check(compared.valid() && compared.first.result.count == 3 && compared.second.result.count == 3 &&
        compared.first.metrics.compatibilityChecks == 3 && compared.second.metrics.compatibilityChecks == 6, "proposal comparison 3 vs 6 checks");
    check(equalArrays(original.start,lesson.start) && equalArrays(original.finish,lesson.finish), "comparison leaves input immutable");
    check(compared.first.medianUs >= 0 && compared.first.medianUs == medianOfSeven(compared.first.samples), "activity median uses its samples");
    ActivityCase empty; check(compareActivities(empty).valid(), "empty activity comparison");
    ActivityTrace trace;
    for (bool dp : {false,true}) {
        auto report = runActivity(lesson,dp,&trace);
        auto plain = runActivity(lesson,dp);
        check(report.valid() && plain.valid() && equalArrays(report.first.result.selected,plain.first.result.selected) &&
            report.first.metrics.compatibilityChecks == plain.first.metrics.compatibilityChecks, "activity trace does not change result or counters");
        std::ostringstream txt,csv;
        check(writeActivityReport(txt,report,false) && txt.str().find("Timing status: not_measured") != std::string::npos &&
            txt.str().find("Selected original IDs: 1 3 4") != std::string::npos, "activity TXT stores real results");
        check(writeActivityReport(csv,report,true) && csv.str().find("\"not_measured\",,0,0") != std::string::npos, "activity untimed CSV fields blank");
        report.first.result.selected[0] = 999;
        std::ostringstream corrupt; check(!writeActivityReport(corrupt,report,true) && corrupt.str().empty(), "activity export revalidates result IDs");
    }
    std::ostringstream csv;
    check(writeActivityReport(csv,compared,true) && csv.str().find("\"compare\",\"DP\"") != std::string::npos &&
        csv.str().find("1:1:3;2:2:5;3:3:4;4:4:6") != std::string::npos, "activity comparison CSV includes both rows and exact input");
    compared.second.timedValid = false;
    check(!compared.valid(), "failed timing result invalidates activity comparison");
    ActivityReport none; std::ostringstream absent;
    check(!writeActivityReport(absent,none,false), "activity report unavailable before run");
    ActivityCase invalid(1); check(!runActivity(invalid,false).available && !compareActivities(invalid).available, "invalid activity never executes via public controller");
    std::ostringstream badOut; badOut.setstate(std::ios::badbit);
    check(!writeActivityCase(badOut,lesson), "activity stream write failure reported");
}
