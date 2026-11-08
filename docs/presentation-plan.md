# Presentation preparation: 12 November 2026

Plan revised 9 November. Targets below distinguish completed sorting from pending work.

| Date | Build target | Understanding / presentation work |
| --- | --- | --- |
| Nov 8 | Merge Sort replay, then begin Quicksort | Hand-trace Merge Sort; explain ownership and replay |
| Nov 9 | Sorting workflow completed; begin activity pair after demo/understanding review | Explain partition/pivot, replay, file validation and counters |
| Nov 10 | Activity pair if feasible, retaining stable sorting release | Check full demo and record actual completed features |
| Nov 11 | Freeze features; fixes and clean rebuild | Create slides from working software; rehearse demo and viva |
| Nov 12 | Present checked version | Offline examples and backup screenshots |

Protected target: two sorting algorithms in Learn and Compare, with input validation,
replay, result checks, case saving/loading and report export. Four algorithms is a
stretch goal; activity Greedy + DP must satisfy the same workflow to count as complete.
Four out of ten solvers means 40% of solver count, not a measured 40% of all software.

Slide outline: problem/users; final scope and actual progress; architecture;
Learn demonstration; Compare evidence; verification/Git history; remaining work.
Use authentic screenshots, commit dates and measured results. Do not present planned
activity selection or other pending algorithms as already implemented.

## 8 November milestone

Implemented recorded Merge Sort learning, current range/depth, buffer display,
next/back/restart/end/menu, bounds, trace-limit handling and replay tests.
Console output intentionally retains prior frames in scrollback for review.
Quicksort was subsequently added with last-element-pivot Lomuto partition, pivot/scan/
boundary display, real/no-op swaps, replay and invariant checks. Both algorithms now
support Learn and result-only Run. Comparison was subsequently added with fresh input
copies, defined operation counts, seven untraced timings per algorithm and a median.
Debug and Release builds and console workflows were checked.

## 9 November sorting completion

Implemented strict versioned TXT save/load and TXT/CSV export from Learn, Run and
Compare. Fresh-session loading works for both algorithms. CSV is read back in tests.
Malformed files leave current state intact, successful loads invalidate stale reports,
existing output files are preserved, and file errors are shown clearly.
The promised sorting workflow is complete within the documented size/trace limits.
Two out of ten featured solvers are complete; this is not a claim of 40% overall completion.
