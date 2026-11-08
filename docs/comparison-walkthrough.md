# Lesson 4: compare algorithms fairly

## Start with a predictable example

Exit any open copy of AlgoGenie (Q from replay, then 0 at the main menu). From PowerShell:

```powershell
cd "C:\Users\User\Desktop\Spl-1 project\AlgoGenie"
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1
.\build\algogenie.exe
```

Choose 3 (Compare), then 4 (sorted input 1 2 3 4 5 6 7 8).
Both outputs must pass sorted order and value preservation checks.

| Metric | Merge Sort | Last-pivot Quicksort |
| --- | ---: | ---: |
| Value comparisons | 12 | 28 |
| Main-array writes | 24 | 0 |
| Merge-buffer writes | 24 | 0 |
| Swaps at distinct indices | 0 | 0 |
| Maximum recursion depth, root 0 | 3 | 7 |

These are checked counts for our specific implementations. They are not universal
counts for all implementations of the algorithms. Neither side reads the expected counts.

## Explain the result

Quicksort picks the last element, which is the largest on this sorted input. The first
partition compares it with seven other values, then six in the next partition, then
five, and so on: 7+6+5+4+3+2+1 = 28 comparisons. Each partition has an empty right side.

Merge Sort splits evenly. For these eight sorted values it uses four comparisons across
the four smallest merges, four across the next two merges, and four in the final merge:
4+4+4 = 12. This count depends on the ordering of the values within those halves.

Why zero Quicksort writes? Every accepted value is already at the boundary. The code
skips self-swaps. The pivot is also already where it belongs. Many comparisons and deep
recursion still occur, so zero writes does not mean zero work or automatically faster.

Do not combine different metrics into one score or claim a universal winner. One method
can perform fewer comparisons and more writes. Timing measures a different question.

## Counting contract

- Value comparisons: only the <= comparison between two keys or a key and the pivot.
  Loop conditions, index checks, validation and result verification do not count.
- Main writes: assignments to elements of the working input array inside the solver.
- Buffer writes: assignments to Merge Sort's temporary merge buffer inside the solver.
  Input cloning, initial zero-fill, scalar variables and trace/replay writes are excluded.
- Swaps: exchanges at distinct indices. Each adds two main writes, even if the values
  happen to be equal. Self-swaps are skipped and contribute no swaps or writes.
- Depth: maximum recursive call depth, with the root at zero and base cases included.

Counts are reset before every run. The original input is retained and cloned separately
for each algorithm. Sorting one algorithm's output with the other would be an unfair test.

## Timing contract and its limits

The program first computes and verifies results in a counted run. It then runs one
unmeasured warm-up per solver and seven timed runs per solver, alternating order.
Each run starts from a fresh copy. Trace recording and count collection are disabled.

The clock starts after cloning input and stops immediately after the solver returns.
Solver-internal allocation and cleanup are included, notably Merge Sort's buffer.
Parsing, input copying, console display, verification and file operations are outside
the measured region. Every measured answer is checked after the clock stops.

Median means the middle of seven measured times after ordering them. It reduces the
effect of an isolated extreme observation, but cannot remove timer resolution limits,
clock-call overhead or all background-machine noise. Times are in microseconds.

For small inputs this machine/toolchain may return zero elapsed clock ticks. The UI
labels a zero median 'not resolved' rather than implying sorting takes no time. Nonzero
displayed precision also does not guarantee measurement accuracy. This bounded lesson
is not a large-scale benchmark; no overall speed winner is declared and timings do not
prove asymptotic complexity. Exact operation counts are the stronger classroom evidence.

Default builds use -O0 -g and are explicitly labelled Debug. To measure an optimized build:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Release
.\build\algogenie-release.exe
```

This uses -O2 and produces a separate executable. The report shows build type, GCC
version and machine name when available. Both modes use C++17 and the same solver code.
The standard chrono steady clock is used; no third-party library is required.

## Read the code in this order

1. src/models/SortMetrics.h: the named counters and their meanings.
2. MergeSort.h and QuickSort.h: find if(metrics) beside each counted operation.
   A null metrics pointer disables collection; it does not choose a different algorithm.
3. src/app/SortComparison.h: countSort creates working copies and verifies outputs;
   timeOne measures a fresh uninstrumented run; compareSorts coordinates the experiment.
4. src/ui/ComparisonView.h: displays verified results and explains measurement limits.

Try input choice 2 ([4,1,3]) next: Merge uses 3 comparisons, 5 main writes and 5 buffer
writes; Quick uses 2 comparisons, 2 swaps and 4 main writes. Both results are [1,3,4].
Then try reversed, equal-value and custom inputs. Counts must reset for every case.

## Validation

Tests include hand-counted examples, counter resets, independently inspected traces,
traced/counted/uninstrumented result equivalence, empty cases and 100-element worst-case
inputs. Timings are checked for validity, not asserted to beat a speed threshold.

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Test
powershell -ExecutionPolicy Bypass -File .\scripts\smoke-test.ps1
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Test -Release
powershell -ExecutionPolicy Bypass -File .\scripts\smoke-test.ps1 -Executable .\build\algogenie-release.exe
```

Next: save/load the exact input and export these reports so the demo can be reproduced.
