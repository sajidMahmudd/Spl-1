#pragma once
#include "ArrayCaseFile.h"
#include "ReportFile.h"
#include "../app/ActivityComparison.h"

namespace algogenie {
inline bool readActivityCase(std::istream& in, ActivityCase& destination, const char*& error) {
    error = nullptr;
    if (in.peek() == 0xEF) {
        in.get();
        if (in.get() != 0xBB || in.get() != 0xBF) { error = "Invalid UTF-8 header."; return false; }
    }
    if (!expectToken(in, "ALGOGENIE") || !expectToken(in, "1") || !expectToken(in, "TYPE") ||
        !expectToken(in, "ACTIVITY") || !expectToken(in, "COUNT")) {
        error = "Expected ALGOGENIE 1 TYPE ACTIVITY COUNT."; return false;
    }
    int n;
    if (!readIntegerToken(in, n) || n < 0 || n > activityLimit) { error = "COUNT must be 0..100."; return false; }
    if (!expectToken(in, "INTERVALS")) { error = "Missing INTERVALS."; return false; }
    ActivityCase candidate(n);
    for (int i = 0; i < n; ++i) {
        if (!readIntegerToken(in, candidate.start[i]) || !readIntegerToken(in, candidate.finish[i]) ||
            candidate.start[i] >= candidate.finish[i]) {
            error = "Each row needs two int values with start < finish."; return false;
        }
    }
    if (!expectToken(in, "END")) { error = "Expected END after exactly COUNT pairs."; return false; }
    char extra[64]; bool overflow;
    if (readToken(in, extra, 64, overflow) || in.bad() || !in.eof()) { error = "Extra data or read failure after END."; return false; }
    destination = candidate; return true;
}
inline bool writeActivityCase(std::ostream& out, const ActivityCase& input) {
    if (!input.valid()) return false;
    out << "ALGOGENIE 1\nTYPE ACTIVITY\nCOUNT " << input.size() << "\nINTERVALS\n";
    for (int i = 0; i < input.size(); ++i) out << input.start[i] << ' ' << input.finish[i] << '\n';
    out << "END\n"; return static_cast<bool>(out);
}
inline bool loadActivityCase(const char* path, ActivityCase& input, const char*& error) {
    std::ifstream in(path, std::ios::binary);
    if (!in) { error = "Cannot open that file."; return false; }
    return readActivityCase(in, input, error);
}
inline bool saveActivityCase(const char* path, const ActivityCase& input, const char*& error) {
    error = nullptr;
    if (!input.valid()) { error = "Invalid activity input."; return false; }
    if (fileExists(path)) { error = "File already exists. Choose a new filename."; return false; }
    std::ofstream out(path);
    if (!out) { error = "Cannot create file; the folder must exist and be writable."; return false; }
    bool ok = writeActivityCase(out, input); out.close();
    if (!ok || out.fail()) { error = "Could not fully write the case."; return false; }
    return true;
}
inline const char* activityMode(const ActivityReport& r) { return r.comparison ? "compare" : r.learning ? "learn" : "run"; }
inline const char* activityTiming(const ActivityReport& r, const ActivityMeasurement& m) {
    return !r.comparison ? "not_measured" : m.medianUs == 0 ? "not_resolved" : "measured";
}
inline void activityIDs(std::ostream& out, const ActivityResult& r) {
    for (int i = 0; i < r.count; ++i) { if (i) out << ' '; out << r.selected[i] + 1; }
}
inline void activityReportRow(std::ostream& out, const ActivityReport& r, const ActivityMeasurement& m, bool dp, bool csv) {
    if (csv) {
        out << "1,"; csvText(out, activityMode(r)); out << ','; csvText(out, dp ? "DP" : "Earliest-finish Greedy");
        out << ',' << r.input.size() << ",\"";
        for (int i = 0; i < r.input.size(); ++i) { if (i) out << ';'; out << i+1 << ':' << r.input.start[i] << ':' << r.input.finish[i]; }
        out << "\",\""; activityIDs(out, m.result);
        out << "\"," << m.result.count << ",true,true," << m.metrics.compatibilityChecks << ',' << m.metrics.dpUpdates
            << ',' << r.preparationComparisons << ',' << r.traceEvents << ',';
        csvText(out, !r.learning ? "not_recorded" : r.traceTruncated ? "truncated" : "complete"); out << ',';
        csvText(out, activityTiming(r,m)); out << ',';
        if (r.comparison) out << m.medianUs;
        out << ',' << (r.comparison ? timingRepetitions : 0) << ',' << (r.comparison ? 1 : 0);
        for (double sample : m.samples) { out << ','; if (r.comparison) out << sample; }
        out << ','; csvText(out, buildLabel()); out << ','; csvText(out, buildId()); out << ','; csvText(out, __VERSION__);
        out << ','; const char* machine = std::getenv("COMPUTERNAME"); csvText(out, machine ? machine : "unknown");
        out << ','; csvText(out, "start<finish; touching allowed; input row IDs; unweighted maximum count");
        out << ','; csvText(out, "compatibility tests only; preprocessing comparisons reported separately");
        out << ','; csvText(out, r.comparison ? "steady_clock; median7; alternate order; presorted immutable input; no trace/count/check/display; returned-result destruction excluded" : "not measured");
        out << '\n';
    } else {
        out << "\nALGORITHM: " << (dp ? "DP (best chain ending at each activity)" : "Earliest-finish Greedy")
            << "\nSelected original IDs: "; activityIDs(out,m.result);
        out << "\nCount: " << m.result.count << "\nNon-overlap and IDs: PASS\nMaximum count: PASS\nCompatibility checks: "
            << m.metrics.compatibilityChecks << "\nDP improvements: " << m.metrics.dpUpdates
            << "\nTiming status: " << activityTiming(r,m) << '\n';
        if (r.comparison) {
            out << "Median us: " << m.medianUs << "\nSeven samples us:";
            for (double sample : m.samples) out << ' ' << sample;
            out << '\n';
        }
    }
}
inline bool writeActivityReport(std::ostream& out, const ActivityReport& r, bool csv) {
    if (!r.valid()) return false;
    out.imbue(std::locale::classic()); out << std::setprecision(17);
    if (csv) out << "schema_version,mode,algorithm,input_count,input_id_start_finish,selected_ids,selected_count,feasible,optimal,"
        << "compatibility_checks,dp_improvements,preparation_comparisons,trace_events,trace_status,timing_status,median_us,repetitions,warmups,"
        << "sample_1_us,sample_2_us,sample_3_us,sample_4_us,sample_5_us,sample_6_us,sample_7_us,build,build_id,compiler,machine,input_rules,counting_rules,timing_method\n";
    else {
        out << "ALGOGENIE ACTIVITY REPORT / VERSION 1\nMode: " << activityMode(r)
            << "\nBuild: " << buildLabel() << "\nBuild ID: " << buildId() << "\nCompiler: " << __VERSION__
            << "\nRules: unweighted maximum count; start < finish; touching endpoints allowed.\n"
            << "IDs are original input rows, starting at 1.\n"
            << "Shared preprocessing: stable insertion sort by finish/start/input ID. O(n^2).\nPreparation comparisons: "
            << r.preparationComparisons << "\nRecorded events: " << r.traceEvents << "\nTrace status: "
            << (!r.learning ? "not_recorded" : r.traceTruncated ? "truncated" : "complete") << '\n';
        writeActivityCase(out,r.input);
    }
    activityReportRow(out,r,r.first,r.comparison ? false : r.dp,csv);
    if (r.comparison) activityReportRow(out,r,r.second,true,csv);
    if (!csv) {
        out << "\nCounts exclude sorting: Greedy n-1 checks (0 if empty); DP n(n-1)/2 checks.\n"
            << "Both maximize count; tied optimal selected sets may differ.\n";
        if (r.comparison) out << "Timing: 1 warm-up + 7 runs each, alternating order; median microseconds.\n"
            << "Same immutable presorted input. Preparation, checks, trace, metrics and display excluded.\n"
            << "Solver allocations/internal cleanup included; returned-result destruction excluded.\n"
            << "Clock: steady_clock. Zero = unresolved; tiny timings are noisy, not a universal speed ranking.\n";
        else out << "Timing not measured in Learn/Run.\n";
    }
    return static_cast<bool>(out);
}
inline bool exportActivityReport(const char* path, const ActivityReport& report, bool csv, const char*& error) {
    error = nullptr;
    if (!report.valid()) { error = "No verified activity report available."; return false; }
    if (fileExists(path)) { error = "File already exists. Choose a new filename."; return false; }
    std::ofstream out(path);
    if (!out) { error = "Cannot create report; folder must exist and be writable."; return false; }
    bool ok = writeActivityReport(out, report, csv); out.close();
    if (!ok || out.fail()) { error = "Could not fully write the report."; return false; }
    return true;
}
}
