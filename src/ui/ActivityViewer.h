#pragma once
#include "SortViewer.h" // Shared strict single-letter command parser.
#include "../app/ActivityComparison.h"
#include "../models/RunReport.h"

namespace algogenie {
inline void printActivities(const ActivityCase& input, const ActivityOrder& order) {
    std::cout << "  Pos    ID       Start      Finish\n";
    for (int i = 0; i < input.size(); ++i) {
        int id = order.ids[i];
        std::cout << std::setw(5) << i << std::setw(6) << id+1 << std::setw(12) << input.start[id] << std::setw(12) << input.finish[id] << '\n';
    }
}
inline const char* activityEventName(ActivityEventKind kind) {
    switch (kind) {
        case ActivityEventKind::GreedyChoice: return "SELECT OR REJECT";
        case ActivityEventKind::DPStart: return "INITIALIZE DP ROW";
        case ActivityEventKind::DPCheck: return "CHECK PREDECESSOR";
        case ActivityEventKind::DPUpdate: return "IMPROVE DP ROW";
        case ActivityEventKind::DPDone: return "ROW COMPLETE";
        case ActivityEventKind::Select: return "RECONSTRUCT SELECTION";
        case ActivityEventKind::Finished: return "COMPLETE";
    }
    return "UNKNOWN";
}
inline void explainActivity(const ActivityCase& input, const ActivityOrder& order, const ActivityReplay& replay) {
    const ActivityEvent* e = replay.current();
    if (!e) { std::cout << "  Initial state after shared finish-time sorting. No solver decisions applied.\n"; return; }
    int id = e->i >= 0 ? order.ids[e->i] : -1;
    switch (e->kind) {
        case ActivityEventKind::GreedyChoice:
            if (e->j < 0) std::cout << "  ID " << id+1 << " finishes earliest: select the first activity.\n";
            else {
                int last = order.ids[e->j];
                std::cout << "  ID " << id+1 << " starts at " << input.start[id] << "; last selected ID " << last+1
                    << " finishes at " << input.finish[last] << ".\n"
                    << (e->compatible ? "  SELECT: start >= last finish.\n" : "  REJECT: overlaps the last selected activity.\n");
            }
            break;
        case ActivityEventKind::DPStart:
            std::cout << "  best[" << e->i << "] = 1: ID " << id+1 << " can form a chain by itself.\n"; break;
        case ActivityEventKind::DPCheck:
            std::cout << "  Can ID " << order.ids[e->j]+1 << " precede ID " << id+1 << "? finish "
                << input.finish[order.ids[e->j]] << " <= start " << input.start[id] << " : " << (e->compatible ? "YES" : "NO") << '\n';
            if (e->compatible) std::cout << "  Candidate best[j]+1 = " << e->value << "; current best[i] = " << replay.best[e->i]
                << ". " << (e->value > replay.best[e->i] ? "Improve at the next event.\n" : "No improvement; keep the earlier predecessor on ties.\n");
            break;
        case ActivityEventKind::DPUpdate:
            std::cout << "  best[" << e->i << "] becomes " << e->value << "; predecessor position = " << e->j << ".\n"; break;
        case ActivityEventKind::DPDone:
            std::cout << "  Best compatible chain ENDING at ID " << id+1 << " has " << e->value << " activities.\n"; break;
        case ActivityEventKind::Select:
            std::cout << "  Follow predecessor links backwards; include ID " << id+1 << ". Display stays chronological.\n"; break;
        case ActivityEventKind::Finished:
            std::cout << "  Maximum selected count: " << e->value << ". Both algorithms solve the unweighted problem.\n"; break;
    }
}
inline void showActivityFrame(const ActivityCase& input, const ActivityOrder& order, const ActivityTrace& trace,
    const ActivityReplay& replay, bool dp) {
    const ActivityEvent* e = replay.current();
    std::cout << "\n====================================================================\n  ALGOGENIE / LEARN / ACTIVITY / "
        << (dp ? "DP" : "EARLIEST-FINISH GREEDY") << "\n  STEP " << replay.position() << " / " << trace.size()
        << " | " << (e ? activityEventName(e->kind) : "INITIAL STATE") << '\n'
        << "  Preparation: finish, then start, then original ID. Sorting is not replayed.\n"
        << "  Pos is zero-based; IDs are original input rows starting at 1.\n";
    printActivities(input,order);
    if (dp) {
        std::cout << "\n  Pos  best  predecessor (-1 = none; best 0 = not visited)\n";
        for (int i = 0; i < input.size(); ++i) std::cout << std::setw(5) << i << std::setw(6) << replay.best[i] << std::setw(8) << replay.predecessor[i] << '\n';
    }
    ActivityResult shown(input.size());
    std::cout << "\n  Selected IDs: [ ";
    for (int i = 0; i < input.size(); ++i) if (replay.selected[i]) {
        std::cout << order.ids[i]+1 << ' '; shown.selected[shown.count++] = order.ids[i];
    }
    std::cout << "]\n  WHY THIS STEP\n"; explainActivity(input,order,replay);
    if (replay.complete()) std::cout << "  Feasible: " << (activityFeasible(input,shown) ? "PASS" : "FAIL")
        << " | Maximum count: " << (activityVerified(input,shown) ? "PASS" : "FAIL") << '\n';
    else if (trace.truncated()) std::cout << "  INCOMPLETE HISTORY: event cap reached; this is not a final-state claim.\n";
    std::cout << "  [N] Next  [B] Back  [R] Restart  [E] End  [Q] Menu (then Enter)\n";
}
inline void learnActivity(const ActivityCase& input, const ActivityTrace& trace, bool dp) {
    ActivityOrder order(input); ActivityReplay replay(input.size(),trace);
    while (true) {
        showActivityFrame(input,order,trace,replay,dp);
        char line[128]; bool tooLong;
        std::cout << "  Command > " << std::flush;
        if (!readLine(std::cin,line,128,tooLong)) return;
        char command = tooLong ? '\0' : replayCommand(line);
        if (command == 'Q') return;
        if (command == 'N') replay.next();
        else if (command == 'B') replay.back();
        else if (command == 'R') replay.restart();
        else if (command == 'E') replay.seek(trace.size());
        else std::cout << "  Enter one command: N, B, R, E or Q.\n";
    }
}
inline void showActivityResult(const ActivityCase& input, const ActivityMeasurement& m, bool dp) {
    std::cout << "\n  " << (dp ? "DP" : "EARLIEST-FINISH GREEDY") << " / VERIFIED RESULT\n  ID       Start      Finish\n";
    for (int i = 0; i < m.result.count; ++i) {
        int id = m.result.selected[i];
        std::cout << std::setw(4) << id+1 << std::setw(12) << input.start[id] << std::setw(12) << input.finish[id] << '\n';
    }
    std::cout << "  Selected count: " << m.result.count << "\n  Non-overlap and IDs: PASS\n  Maximum count: PASS\n"
        << "  Compatibility checks: " << m.metrics.compatibilityChecks << " | DP improvements: " << m.metrics.dpUpdates << '\n';
}
inline bool showActivityReport(const ActivityReport& report) {
    if (!report.valid()) { std::cout << "  INTERNAL CHECK FAILED: activity result rejected.\n"; return false; }
    std::cout << "\n====================================================================\n  ALGOGENIE / ACTIVITY / "
        << (report.comparison ? "COMPARE" : "RUN") << "\n  Goal: maximum COUNT, no weights. Touching endpoints are allowed.\n";
    showActivityResult(report.input,report.first,report.comparison ? false : report.dp);
    if (report.comparison) {
        showActivityResult(report.input,report.second,true);
        std::cout << "\n  Matching optimal counts: PASS (selected IDs may differ on ties)\n"
            << "  GREEDY vs DP: " << report.first.metrics.compatibilityChecks << " vs " << report.second.metrics.compatibilityChecks << " compatibility checks\n"
            << "  SEPARATE TIMING / median of 7 / microseconds\n";
        const auto flags = std::cout.flags(); const auto precision = std::cout.precision();
        std::cout << std::fixed << std::setprecision(3);
        if (report.first.medianUs == 0) std::cout << "  Greedy: not resolved\n";
        else std::cout << "  Greedy: " << report.first.medianUs << '\n';
        if (report.second.medianUs == 0) std::cout << "  DP: not resolved\n";
        else std::cout << "  DP: " << report.second.medianUs << '\n';
        std::cout.flags(flags); std::cout.precision(precision);
        std::cout << "  1 warm-up + 7 runs each; alternate order; same immutable presorted input.\n"
            << "  Excludes sorting, tracing, counting, verification, display and returned-result destruction.\n"
            << "  Includes solver allocation/internal cleanup. All timed results checked.\n"
            << "  Tiny timings may be noisy/unresolved; no universal speed winner.\n";
    }
    std::cout << "  Shared preparation comparisons: " << report.preparationComparisons << " (excluded from solver checks)\n"
        << "  Preparation: insertion sort O(n^2). Presorted solvers: Greedy O(n), DP O(n^2).\n"
        << "  Solver storage O(n) including output; replay/history are additional.\n"
        << "  Build: " << buildLabel() << '\n';
    return true;
}
}
