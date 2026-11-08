# Supervisor demonstration: completed sorting workflow

Checked milestone: 9 November 2026. Merge Sort and last-pivot Lomuto Quicksort support
custom input, validation, learning replay, result verification, comparison, case save/load
and TXT/CSV report export. Activity selection and the other six solvers are still pending.

## Prepare

Open the AlgoGenie folder in VS Code. Exit any already-running copy before compiling.

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Release
.\build\algogenie-release.exe
```

Default debug builds also work. Use the Release executable when showing timing.
Keep learning inputs small and focus on the exact operation counts if timing is unresolved.

## Demonstration (about 7 minutes)

1. Learn Merge Sort: menu 1, algorithm 1, input 2 ([4,1,3]). Advance with N, explain a
   split and the temporary buffer, move Back, Restart, then End. Show both PASS labels.
   Q returns to the main menu.
2. Learn Quicksort: menu 1, algorithm 2, input 2. Press N six times. Explain pivot 3,
   leaving 4 on the right, swapping 1 into the left region, and placing the pivot.
   Back restores the previous state. End shows the verified result. Q returns.
3. Compare: menu 3, input 4 (eight sorted values). Explain 12 versus 28 comparisons,
   balanced versus one-sided recursion, and why zero Quicksort writes is possible.
4. Save input: menu 4, option 1, path saved/supervisor-case.txt.
5. Export TXT: menu 5, option 1, path reports/supervisor-report.txt.
6. Export CSV: menu 5, option 2, path reports/supervisor-report.csv.
   Open these files in VS Code and show that the exported counts match the screen.
7. Exit with 0 and restart the executable. Load: menu 4, option 2,
   saved/supervisor-case.txt. Compare: menu 3, input 7. The same answers and operation
   counts return; timing may vary. The loaded case also works in Learn for either solver.

Use new filenames on a repeated rehearsal; existing files are intentionally preserved.
To show invalid-input handling, choose a custom array and enter an invalid token, or
try loading a missing file. Explain the error and recover through the menu.

## Evidence to show

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Test
powershell -ExecutionPolicy Bypass -File .\scripts\smoke-test.ps1
powershell -ExecutionPolicy Bypass -File .\scripts\persistence-test.ps1
```

The C++ suite passes 29,575 assertions (not 29,575 distinct test functions). Checks
include exhaustive short arrays, integer boundaries, loop invariants, replay restoration,
metric contracts and strict file/report validation. Console tests exercise modes and
file workflows for both algorithms, parse CSV back independently, and restart the program.
The same suites are also checked under -O2. Generated integration artifacts stay in build/.

## A precise progress statement

"The sorting module is implemented end to end for Merge Sort and Quicksort: users can
enter or load input, replay decisions, verify results, compare work, save cases and export
reports. It works offline within documented input limits. I am now moving to activity selection."

Do not describe the entire AlgoGenie project as finished or claim four completed solvers.

## Questions to prepare

- Why preserve the original input? It supports independent result checks, fair comparisons
  and reconstruction of replay states.
- How does Back work? Reset displayed state and apply a shorter event prefix.
- Why check both order and value preservation? Sorted data can still contain lost/extra items.
- Why can Quicksort degrade? A fixed pivot can repeatedly produce an empty side.
- Why separate counters and timing? Instrumentation and display change execution cost.
- What is stored in a case? Only the versioned problem input. Reports include the answers
  and measurement settings; deterministic replay is regenerated when the case runs again.
- What happens if a file is malformed? It is rejected without replacing the current case.
- Are timing results always meaningful? No; tiny inputs can fall below clock resolution.
- What are the limits? Learn 30 values, Run/Compare/load 100; trace history 20,000 events.

Read learning-mode-walkthrough.md, quicksort-walkthrough.md and comparison-walkthrough.md
for the algorithm-specific explanations before memorizing any presentation wording.
