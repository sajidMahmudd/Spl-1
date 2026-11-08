#pragma once
#include "../src/io/ReportFile.h"

inline void rejectedCase(const char* text) {
    IntArray destination(2); destination[0] = 9; destination[1] = -4;
    std::istringstream in(text); const char* error = nullptr;
    check(!algogenie::readArrayCase(in, destination, error) && error && destination.size() == 2 &&
        destination[0] == 9 && destination[1] == -4, "invalid case rejected without changing current input");
}
inline void fileChecks() {
    for (int n : {0, 1, 30, 100}) {
        IntArray input(n);
        for (int i = 0; i < n; ++i) input[i] = i % 3 == 0 ? INT_MIN : i % 3 == 1 ? INT_MAX : -i;
        std::stringstream file; const char* error = nullptr; IntArray restored;
        check(algogenie::writeArrayCase(file, input), "case serialization succeeds");
        check(algogenie::readArrayCase(file, restored, error) && equalArrays(input, restored), "case roundtrip preserves size and exact values");
    }
    rejectedCase("");
    rejectedCase("ALGOGENIE 2 TYPE ARRAY COUNT 0 VALUES END");
    rejectedCase("ALGOGENIE 1 TYPE GRAPH COUNT 0 VALUES END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT -1 VALUES END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 101 VALUES END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 99999999999999999999999999 VALUES END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 2 VALUES 1 END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 1 VALUES 1 2 END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 1 VALUES 1");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 1 VALUES 1 END extra");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 1 VALUES 1.5 END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 1 VALUES 2147483648 END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 1 VALUES -2147483649 END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 1 VALUES + END");
    rejectedCase("ALGOGENIE 1 TYPE ARRAY COUNT 1 VALUES 0000000000000000000000000000000000000000000000000000000000000000000000000000000000001 END");
    std::istringstream bom("\xEF\xBB\xBF" "ALGOGENIE 1\r\nTYPE\tARRAY\r\nCOUNT 1 VALUES -3 END\r\n");
    IntArray restored; const char* error = nullptr;
    check(algogenie::readArrayCase(bom, restored, error) && restored[0] == -3, "UTF-8 BOM and Windows whitespace supported");
    std::istringstream badIo("ALGOGENIE 1 TYPE ARRAY COUNT 0 VALUES END"); badIo.setstate(std::ios::badbit);
    check(!algogenie::readArrayCase(badIo, restored, error), "stream read failure rejected");
    std::string nulToken = "ALGOGENIE"; nulToken.push_back('\0'); nulToken += "hidden 1 TYPE ARRAY COUNT 0 VALUES END";
    std::istringstream binary(nulToken);
    check(!algogenie::readArrayCase(binary, restored, error), "embedded NUL cannot hide a token suffix");
    std::ostringstream badOutput; badOutput.setstate(std::ios::badbit);
    check(!algogenie::writeArrayCase(badOutput, restored), "stream write failure reported");
    check(!algogenie::loadArrayCase("build/__missing_test_folder__/no-case.txt", restored, error) && error,
        "missing file has useful error");
    check(!algogenie::saveArrayCase("build/__missing_test_folder__/case.txt", restored, error) && error,
        "missing output directory has useful error");

    algogenie::RunReport report; report.available = true; report.learning = true;
    report.input = IntArray(3); report.input[0] = 4; report.input[1] = 1; report.input[2] = 3;
    algogenie::countSort(report.input, false, report.first); report.traceEvents = 23;
    std::ostringstream txt, csv;
    check(algogenie::writeTxtReport(txt, report) && txt.str().find("Value comparisons: 3") != std::string::npos &&
        txt.str().find("Timing status: not_measured") != std::string::npos &&
        txt.str().find("Recorded events: 23") != std::string::npos, "learning TXT exports real counts and no fabricated timing");
    check(algogenie::writeCsvReport(csv, report) && csv.str().find("\"learn\",\"Merge Sort\"") != std::string::npos &&
        csv.str().find("\"not_measured\",,0,0") != std::string::npos, "learning CSV has blank timing and zero repetitions");
    std::ostringstream quoting; algogenie::csvText(quoting, "a,\"b\"\nc");
    check(quoting.str() == "\"a,\"\"b\"\"\nc\"", "CSV quotes comma quote and newline correctly");
    report.traceTruncated = true; std::ostringstream truncated;
    check(algogenie::writeTxtReport(truncated, report) && truncated.str().find("Trace status: truncated") != std::string::npos,
        "report exposes incomplete trace while preserving valid final result");
    const auto pair = algogenie::compareSorts(report.input);
    report.comparison = true; report.learning = false; report.traceEvents = 0;
    report.first = pair.merge; report.second = pair.quick;
    std::ostringstream compared;
    check(algogenie::writeCsvReport(compared, report) && compared.str().find("\"Quicksort\"") != std::string::npos &&
        compared.str().find("\"Merge Sort\"") != std::string::npos, "comparison CSV contains both algorithm rows");
    report.first.result[0] = 999;
    std::ostringstream corrupted;
    check(!algogenie::writeTxtReport(corrupted, report) && corrupted.str().empty(), "export rejects a corrupted result even with stale PASS flags");
    algogenie::RunReport noRun; std::ostringstream absent;
    check(!algogenie::writeCsvReport(absent, noRun), "no-report export is rejected");
}
