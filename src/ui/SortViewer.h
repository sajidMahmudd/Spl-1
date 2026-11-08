#pragma once
#include <iomanip>
#include "Console.h"
#include "../trace/SortReplay.h"
#include "../verify/Sorting.h"

inline const char* eventName(SortEventKind kind) {
    switch (kind) {
        case SortEventKind::Split: return "SPLIT";
        case SortEventKind::BaseCase: return "BASE CASE";
        case SortEventKind::BeginMerge: return "BEGIN MERGE";
        case SortEventKind::Compare: return "COMPARE";
        case SortEventKind::BufferWrite: return "WRITE BUFFER";
        case SortEventKind::CopyBack: return "COPY BACK";
        case SortEventKind::MergeDone: return "RANGE SORTED";
        case SortEventKind::Finished: return "COMPLETE";
        case SortEventKind::ChoosePivot: return "CHOOSE PIVOT";
        case SortEventKind::ComparePivot: return "COMPARE WITH PIVOT";
        case SortEventKind::PartitionSwap: return "ACCEPT INTO LEFT REGION";
        case SortEventKind::KeepRight: return "KEEP IN RIGHT REGION";
        case SortEventKind::PivotPlaced: return "PLACE PIVOT";
        case SortEventKind::PartitionDone: return "RANGE SORTED";
    }
    return "UNKNOWN";
}
inline void explainEvent(const SortReplay& replay) {
    const SortEvent* e = replay.current();
    if (!e) {
        std::cout << "  This is the original array. Press N to inspect the first decision.\n";
        return;
    }
    switch (e->kind) {
        case SortEventKind::Split:
            std::cout << "  Split [" << e->begin << ',' << e->end << ") at index " << e->middle
                      << ". Sort the left half, then the right half.\n"; break;
        case SortEventKind::BaseCase:
            std::cout << "  This range has " << e->end - e->begin << " element(s). It is already sorted.\n"; break;
        case SortEventKind::BeginMerge:
            std::cout << "  Both halves are sorted. Start a fresh visible merge buffer.\n"; break;
        case SortEventKind::Compare:
            std::cout << "  Compare left value " << replay.values[e->left] << " (index " << e->left
                      << ") with right value " << replay.values[e->right] << " (index " << e->right << ").\n"
                      << "  Choose the smaller value; on a tie choose the left for stability.\n"; break;
        case SortEventKind::BufferWrite:
            std::cout << "  Place " << e->value << " from array[" << e->left << "] into buffer[" << e->index << "].\n"
                      << "  After one half is exhausted, copy remaining values from the other.\n"; break;
        case SortEventKind::CopyBack:
            std::cout << "  Copy buffer[" << e->index << "] = " << e->value << " into array[" << e->index << "].\n"
                      << "  During copy-back, the main array alone may temporarily repeat a value.\n"; break;
        case SortEventKind::MergeDone:
            std::cout << "  The entire active range is now sorted. Return to its parent call.\n"; break;
        case SortEventKind::Finished:
            std::cout << "  All recursive calls have returned. Check the final reconstructed result.\n"; break;
        case SortEventKind::ChoosePivot:
            std::cout << "  Choose the last element: pivot " << e->value << " at index " << e->index << ".\n"
                      << "  Boundary starts at " << e->middle << "; the <= pivot region is empty.\n"; break;
        case SortEventKind::ComparePivot:
            std::cout << "  Compare array[" << e->right << "] = " << replay.values[e->right]
                      << " with pivot " << e->value << ".\n"
                      << "  Values <= pivot enter the left region; greater values stay right.\n"; break;
        case SortEventKind::PartitionSwap:
            if (e->left == e->right)
                std::cout << "  Already at the boundary: no array write is needed.\n";
            else
                std::cout << "  Swap indices " << e->left << " and " << e->right << " to extend the <= pivot region.\n";
            std::cout << "  Advance the boundary to " << e->middle << ".\n"; break;
        case SortEventKind::KeepRight:
            std::cout << "  " << replay.values[e->right] << " > " << e->value
                      << ": leave this value in the scanned right region.\n"
                      << "  The boundary stays at " << e->middle << "; continue scanning.\n"; break;
        case SortEventKind::PivotPlaced:
            if (e->left == e->right) std::cout << "  The pivot is already at the partition boundary.\n";
            else std::cout << "  Swap the last element with the boundary to place the pivot.\n";
            std::cout << "  Pivot " << e->value << " is final at index " << e->index
                      << ". Sort both sides, excluding the pivot.\n"; break;
        case SortEventKind::PartitionDone:
            std::cout << "  Both recursive sides are sorted. This entire range is now sorted.\n"; break;
    }
}
inline void showFrame(const IntArray& original, const SortTrace& trace, const SortReplay& replay, bool quick) {
    const SortEvent* e = replay.current();
    const bool partition = e && (e->kind == SortEventKind::ChoosePivot || e->kind == SortEventKind::ComparePivot ||
        e->kind == SortEventKind::PartitionSwap || e->kind == SortEventKind::KeepRight || e->kind == SortEventKind::PivotPlaced);
    std::cout << "\n====================================================================\n"
              << "  ALGOGENIE / LEARN / " << (quick ? "QUICKSORT" : "MERGE SORT") << '\n'
              << "  STEP " << replay.position() << " / " << trace.size() << "  |  "
              << (e ? eventName(e->kind) : "INITIAL STATE") << '\n'
              << "--------------------------------------------------------------------\n";
    if (quick) std::cout << "  Lomuto partition | Last-element pivot\n";
    if (e) {
        std::cout << "  Active [" << e->begin << ',' << e->end << ")  |  Recursion depth " << e->depth;
        if (!quick) std::cout << "  |  Middle " << e->middle;
        std::cout << '\n';
    }
    if (partition) std::cout << "  Pivot " << e->value << " at index " << e->index
                             << "  |  Boundary " << e->middle << '\n';
    std::cout << "  Indices start at 0. [begin,end) excludes end. * = active range.\n";
    // Wrap into groups to keep 30 elements and full-width integers legible.
    for (int start = 0; start < replay.values.size(); start += 4) {
        int end = start + 4; if (end > replay.values.size()) end = replay.values.size();
        std::cout << "\n  Index   ";
        for (int i = start; i < end; ++i) std::cout << std::setw(12) << i;
        std::cout << "\n  Active  ";
        for (int i = start; i < end; ++i) std::cout << std::setw(12) << (e && i >= e->begin && i < e->end ? "*" : ".");
        std::cout << "\n  Array   ";
        for (int i = start; i < end; ++i) std::cout << std::setw(12) << replay.values[i];
        if (!quick) {
            std::cout << "\n  Buffer  ";
            for (int i = start; i < end; ++i) {
                if (replay.bufferValid[i]) std::cout << std::setw(12) << replay.buffer[i];
                else std::cout << std::setw(12) << ".";
            }
        } else if (partition) {
            std::cout << "\n  Pivot   ";
            for (int i = start; i < end; ++i) std::cout << std::setw(12) << (i == e->index ? "P" : ".");
            std::cout << "\n  Boundary";
            for (int i = start; i < end; ++i) std::cout << std::setw(12) << (i == e->middle ? "B" : ".");
            if (e->kind == SortEventKind::ComparePivot || e->kind == SortEventKind::KeepRight) {
                std::cout << "\n  Scan    ";
                for (int i = start; i < end; ++i) std::cout << std::setw(12) << (i == e->right ? "S" : ".");
            }
        }
        std::cout << '\n';
    }
    if (replay.values.size() == 0) std::cout << "\n  Array: [ ]\n";
    std::cout << "\n  WHY THIS STEP\n"; explainEvent(replay);
    if (replay.complete()) {
        std::cout << "\n  Sorted order     : " << (isSorted(replay.values) ? "PASS" : "FAIL")
                  << "\n  Values preserved : " << (preservesValues(original, replay.values) ? "PASS" : "FAIL") << '\n';
    } else if (trace.truncated()) {
        std::cout << "\n  INCOMPLETE HISTORY: event limit reached. This is not a final-state claim.\n";
    }
    std::cout << "\n  [N] Next   [B] Back   [R] Restart   [E] End   [Q] Menu\n"
              << "====================================================================\n";
}
inline char replayCommand(const char* line) {
    while (space(*line)) ++line;
    char command = *line;
    if (!command) return '\0';
    ++line;
    while (space(*line)) ++line;
    if (*line) return '\0';
    if (command >= 'a' && command <= 'z') command = static_cast<char>(command - 'a' + 'A');
    return command;
}
inline void learnSort(const IntArray& original, const SortTrace& trace, bool quick) {
    SortReplay replay(original, trace);
    while (true) {
        showFrame(original, trace, replay, quick);
        char line[128]; bool tooLong;
        std::cout << "  Command > " << std::flush;
        if (!readLine(std::cin, line, 128, tooLong)) return;
        const char command = tooLong ? '\0' : replayCommand(line);
        if (command == 'Q') return;
        if (command == 'N') replay.next();
        else if (command == 'B') replay.back();
        else if (command == 'R') replay.restart();
        else if (command == 'E') replay.seek(trace.size());
        else std::cout << "  Enter one command: N, B, R, E or Q, then press Enter.\n";
    }
}
