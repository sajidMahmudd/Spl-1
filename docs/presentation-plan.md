# Presentation preparation: 12 November 2026

Plan revised 8 November. Target, not a claim that pending work is implemented.

| Date | Build target | Understanding / presentation work |
| --- | --- | --- |
| Nov 8 | Merge Sort replay, then begin Quicksort | Hand-trace Merge Sort; explain ownership and replay |
| Nov 9 | Quicksort learning and sorting comparison | Explain partition/pivot, define counters; collect real outputs |
| Nov 10 | Finish sorting save/load and reports; activity pair only if the sorting workflow is stable | Check full demo and record actual completed features |
| Nov 11 | Freeze features; fixes and clean rebuild | Create slides from working software; rehearse demo and viva |
| Nov 12 | Present checked version | Offline examples and backup screenshots |

Protected target: two sorting algorithms in Learn and Compare, with input validation,
replay, result checks, case saving/loading and report export. Four algorithms is a
stretch goal; activity Greedy + DP must satisfy the same workflow to count as complete.
Four out of ten solvers means 40% of solver count, not a measured 40% of all software.

Slide outline: problem/users; final scope and actual progress; architecture;
Learn demonstration; Compare evidence; verification/Git history; remaining work.
Use authentic screenshots, commit dates and measured results. Do not present planned
activity selection, comparison or persistence as already implemented.

## 8 November milestone

Implemented recorded Merge Sort learning, current range/depth, buffer display,
next/back/restart/end/menu, bounds, trace-limit handling and replay tests.
Console output intentionally retains prior frames in scrollback for review.
Current algorithm support: Merge Sort only. Comparison and persistence remain pending.
