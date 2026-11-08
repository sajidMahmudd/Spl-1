# First target: Merge Sort with verified results

Historical milestone from November 4. Learning replay was added November 8;
see learning-mode-walkthrough.md for the current lesson and controls.

Implemented: manually owned integer array with deep copies, handwritten Merge Sort,
strict integer input, sample case, repeated runs, and separate result checks.
The integer array has a fixed size selected at runtime; growable storage is future work.
Not implemented yet: learning replay, Quicksort, comparison, persistence or reports.

## Run from the VS Code terminal

Open the AlgoGenie folder and choose Terminal > New Terminal. Use PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1
.\build\algogenie.exe
```

In the current menu choose 2 (Run), then 1 (Merge Sort), then 3 (duplicate example).
Expected output: `[ 1 3 3 5 8 ]` and two PASS labels.
For custom input choose 2 (Run), then 1 (Merge Sort), then 1 (custom input).
Size zero is supported. Use 0 at the main menu to exit.

Run the checks:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Test
```

## Understand it in this order

1. `src/main.cpp`: input -> independent working copy -> sort -> verify -> display.
2. `src/core/IntArray.h`: new[] creates storage; the destructor releases it;
   copying allocates separate storage so the original input stays intact.
3. `src/algorithms/sorting/MergeSort.h`: ranges use [begin,end). Split at the
   middle; recursively sort both halves; merge the smallest remaining values.
4. `src/verify/Sorting.h`: check adjacent order and independently count values.
5. `src/io/Input.h`: reject missing/extra tokens, decimals and overflow.

## Hand trace before your viva

For [8,3,5,3,1], our midpoint rule splits into [8,3] and [5,3,1].
The sorted halves are [3,8] and [1,3,5]. Merge chooses 1,3,3,5,8.
On equal values, choose the left half first. The merge invariant is that the
written buffer prefix contains the smallest consumed elements in sorted order.

Explain why recursion stops at fewer than two values, why a temporary buffer is
needed, and why a sorted output alone would not detect a lost duplicate.

Solver complexity: O(n log n) time and O(n) auxiliary storage. Frequency checking
is O(n^2), separate from the solver, with console input limited to 100 elements.
This milestone's tests verify integer values; tagged-element stability tests are
not included yet. Essential standard I/O and allocation are used, no STL container
or library sort. The test harness additionally uses string streams to test input.

Next target: record merge events and implement next/back/restart learning replay.
