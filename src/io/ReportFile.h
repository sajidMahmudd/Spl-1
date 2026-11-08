#pragma once
#include <iomanip>
#include <cstdlib>
#include <locale>
#include "ArrayCaseFile.h"
#include "../models/RunReport.h"

namespace algogenie {
inline void writeValues(std::ostream& out, const IntArray& values) {
    for (int i = 0; i < values.size(); ++i) { if (i) out << ' '; out << values[i]; }
}
// RFC-style quoting: escape embedded quotes, commas and newlines in text cells.
inline void csvText(std::ostream& out, const char* text) {
    out << '"';
    for (const char* p = text; *p; ++p) { if (*p == '"') out << '"'; out << *p; }
    out << '"';
}
inline const char* traceStatus(const RunReport& report) {
    return !report.learning ? "not_recorded" : report.traceTruncated ? "truncated" : "complete";
}
inline const char* timingStatus(const RunReport& report, const SortMeasurement& measurement) {
    return !report.comparison ? "not_measured" : measurement.medianUs == 0 ? "not_resolved" : "measured";
}
inline void txtMeasurement(std::ostream& out, const RunReport& report, const SortMeasurement& result, bool quick) {
    out << "\nALGORITHM: " << (quick ? "Quicksort (Lomuto, last-element pivot)" : "Merge Sort (stable tie handling)")
        << "\nOutput: [ "; writeValues(out, result.result);
    out << " ]\nSorted order: PASS\nValues preserved: PASS\n"
        << "Value comparisons: " << result.metrics.comparisons
        << "\nMain-array writes: " << result.metrics.mainWrites
        << "\nMerge-buffer writes: " << result.metrics.bufferWrites
        << "\nDistinct-index swaps: " << result.metrics.swaps
        << "\nMaximum recursion depth: " << result.metrics.maximumDepth
        << "\nTiming status: " << timingStatus(report, result) << '\n';
    if (report.comparison) {
        out << "Median (microseconds; zero means not resolved): " << result.medianUs << "\nSamples (microseconds):";
        for (double sample : result.samplesUs) out << ' ' << sample;
        out << '\n';
    }
}
inline bool writeTxtReport(std::ostream& out, const RunReport& report) {
    if (!report.valid()) return false;
    out.imbue(std::locale::classic()); out << std::setprecision(17);
    out << "ALGOGENIE SORTING REPORT / VERSION 1\nMode: " << reportMode(report)
        << "\nBuild: " << buildLabel() << "\nBuild ID: " << buildId() << "\nCompiler: GCC " << __VERSION__ << " / C++17\n";
    const char* machine = std::getenv("COMPUTERNAME");
    out << "Machine: " << (machine ? machine : "unknown") << "\nInput count: " << report.input.size() << "\nInput: [ ";
    writeValues(out, report.input); out << " ]\nTrace status: " << traceStatus(report)
        << "\nRecorded events: " << report.traceEvents << '\n';
    txtMeasurement(out, report, report.first, report.comparison ? false : report.quick);
    if (report.comparison) {
        txtMeasurement(out, report, report.second, true);
        out << "\nMatching sorted outputs: PASS\n";
    }
    out << "\nCOUNTING RULES\nValue comparisons exclude loop/index checks. Main/buffer writes are solver assignments;\n"
        << "input cloning, zero-fill, scalar and replay writes are excluded. Distinct-index swaps\n"
        << "contribute two main writes; self-swaps are skipped. Root recursion depth is zero.\n";
    if (report.comparison) out << "\nTIMING METHOD\nOne warm-up and seven measured runs per algorithm; median of seven; alternating order.\n"
        << "Fresh input copies for every run. Copying, validation, display, trace collection,\n"
        << "metrics collection and file writing are excluded. Internal solver allocation and\n"
        << "cleanup are included. All timed answers were verified after stopping the clock.\n"
        << "Clock: std::chrono::steady_clock. Small-input results can be unresolved/noisy;\n"
        << "no universal speed winner or complexity proof is claimed. Use Release for experiments.\n";
    else out << "\nTiming was not measured for this Learn/Run session. Use Compare for separate timings.\n";
    out << "\nREPRODUCIBLE INPUT (save separately as a case file)\n";
    writeArrayCase(out, report.input);
    return static_cast<bool>(out);
}
inline void csvMeasurement(std::ostream& out, const RunReport& report, const SortMeasurement& result, bool quick) {
    out << "1,"; csvText(out, reportMode(report)); out << ',';
    csvText(out, quick ? "Quicksort" : "Merge Sort"); out << ',';
    csvText(out, quick ? "Lomuto last-element pivot" : "Stable merge");
    out << ',' << report.input.size() << ",\""; writeValues(out, report.input);
    out << "\",\""; writeValues(out, result.result);
    out << "\",true,true," << result.metrics.comparisons << ',' << result.metrics.mainWrites << ','
        << result.metrics.bufferWrites << ',' << result.metrics.swaps << ',' << result.metrics.maximumDepth
        << ',' << report.traceEvents << ','; csvText(out, traceStatus(report)); out << ',';
    csvText(out, timingStatus(report, result)); out << ',';
    if (report.comparison) out << result.medianUs;
    out << ',' << (report.comparison ? timingRepetitions : 0) << ',' << (report.comparison ? 1 : 0);
    for (double sample : result.samplesUs) { out << ','; if (report.comparison) out << sample; }
    out << ','; csvText(out, buildLabel()); out << ','; csvText(out, buildId());
    out << ','; csvText(out, __VERSION__);
    const char* machine = std::getenv("COMPUTERNAME"); out << ','; csvText(out, machine ? machine : "unknown");
    out << ','; csvText(out, "value comparisons; solver writes only; self-swaps excluded; root depth zero");
    out << ','; csvText(out, report.comparison ? "steady_clock; alternate order; fresh copies; internal allocation included; no trace/count collection" : "not measured");
    out << '\n';
}
inline bool writeCsvReport(std::ostream& out, const RunReport& report) {
    if (!report.valid()) return false;
    out.imbue(std::locale::classic()); out << std::setprecision(17);
    out << "schema_version,mode,algorithm,variant,input_count,input_values,output_values,sorted,values_preserved,"
        << "comparisons,main_writes,buffer_writes,swaps,max_depth,trace_events,trace_status,timing_status,median_us,"
        << "repetitions,warmups,sample_1_us,sample_2_us,sample_3_us,sample_4_us,sample_5_us,sample_6_us,sample_7_us,"
        << "build,build_id,compiler,machine,counting_rules,timing_method\n";
    csvMeasurement(out, report, report.first, report.comparison ? false : report.quick);
    if (report.comparison) csvMeasurement(out, report, report.second, true);
    return static_cast<bool>(out);
}
inline bool exportReport(const char* path, const RunReport& report, bool csv, const char*& error) {
    error = nullptr;
    if (!report.valid()) { error = "No verified report is available. Run an algorithm first."; return false; }
    if (fileExists(path)) { error = "File already exists. Choose a new filename."; return false; }
    std::ofstream out(path);
    if (!out) { error = "Cannot create the report. The folder must exist and be writable."; return false; }
    const bool written = csv ? writeCsvReport(out, report) : writeTxtReport(out, report);
    out.close();
    if (!written || out.fail()) { error = "Report could not be fully written."; return false; }
    return true;
}
}
