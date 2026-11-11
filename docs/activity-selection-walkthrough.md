# Activity Selection: tonight's code-reading and demonstration guide

## What you can demonstrate now

Main menu **6** opens the Activity Selection lab. Its own menu is:

1. Learn (choose Greedy or DP)
2. Run (selected activities and checks)
3. Compare (both algorithms automatically)
4. Save/load an activity TXT case
5. Export the latest activity TXT or CSV report
0. Return to the main menu

Sorting still uses the original main-menu options. Sorting and activity inputs/reports
are independent and survive switching between the two labs during the same session.
There are now four featured solvers implemented: Merge Sort, Quicksort, activity
Greedy and activity DP. Coin change and the four graph solvers remain planned.
This is 4 of 10 solvers, not a measured percentage of total project work.

## First understand the problem

An activity has a start time and a finish time. Choose the **maximum number** of
compatible activities for one resource. This is the unweighted problem: there are
no profits or priorities. A later activity may begin exactly when the previous one
finishes (`next.start >= last.finish`). Every individual interval must satisfy
`start < finish`; zero-duration and reversed intervals are rejected.

Signed integer times are supported. IDs are the **original input row numbers**, starting
at 1. Input rows are never reordered or overwritten: a separate index array describes
finish-time order. Two identical intervals are distinct activity IDs, but they overlap.

The lesson is your proposal's A/B/C/D example, labelled 1/2/3/4:

| Original ID | Start | Finish |
|---|---:|---:|
| 1 (A) | 1 | 3 |
| 2 (B) | 2 | 5 |
| 3 (C) | 3 | 4 |
| 4 (D) | 4 | 6 |

Finish-sorted positions are `[1,3,2,4]` in terms of original IDs.
Both methods select `[1,3,4]`, count **3**. IDs and sorted positions are different!

## A two-minute demo

Launch `build/algogenie.exe`. Enter each choice on its own line, followed by Enter.

- **Greedy Learn:** `6`, `1`, `1`, `2`. Use `N`, `B`, `R`, `E`, `Q`.
  There are 5 recorded events: four select/reject decisions and completion.
- After `Q`, you are in the activity menu. **DP Learn:** `1`, `2`, `2`.
  Watch the best/predecessor table. `E` reaches the checked selection; `Q` returns.
  This example has 21 events, including reconstruction.
- **Compare:** `3`, `2`. Both select 3; Greedy has 3 compatibility checks; DP has 6.
- **Save:** `4`, `1`, then a new path such as `saved/activity-demo.txt`.
- **CSV:** `5`, `2`, then a new path such as `reports/activity-demo.csv`.
- **Load:** `4`, `2`, `examples/activity-proposal.txt`. A successful load clears
  the old activity report. Run Compare `3`, then input `6` to reuse this case.
- `0` returns to the main menu. Another `0` exits.

Input option **6** means current/loaded activity case; sorting still uses input **7**.
Do not confuse these with main menu **6**, which opens the activity lab.

## Study these files in this order

Keep VS Code open. Search the named function with Ctrl+Shift+F. Read one section,
explain it aloud, and trace the four-activity lesson before going on.

### 1. `src/models/Activity.h`: the data

- `ActivityCase` owns `start` and `finish` arrays of the same length. `valid()` checks
  the 100-activity bound, matching lengths and strict start-before-finish.
- `ActivityOrder` owns `ids`, a list of original **zero-based** array indices.
  It uses handwritten insertion sort, ordered by finish, then start, then original ID
  through stability. Sorting the indices preserves input identity and input values.
- `comparisons` counts ordering-comparator evaluations during this preparation.
- `ActivityMetrics` counts solver compatibility tests and successful DP improvements.
- `ActivityResult.selected` has capacity n; only its first `count` entries are answers.
  Values are original zero-based indices, printed with `+1` as user-facing IDs.

Possible question: **Why not sort the original arrays independently?** That would
break start/finish pairs and identity. Sorting an index list keeps each pair together.
Possible change: increase limits here, but also consider display size, trace capacity,
quadratic DP/preparation/checking, prompts and boundary tests.

### 2. `src/algorithms/activity/ActivitySelection.h`: the two decisions

`activityGreedy` scans finish-sorted positions. `last = -1` means nothing selected.
Accept the first activity, then accept a candidate only when its start is at least
the last selected finish. On acceptance append its original ID and update `last`.
The explicit no-selection state avoids using a time sentinel that would fail for
negative or extreme integer times.

Lesson: select ID 1; select ID 3 because 3 >= 3; reject ID 2 because 2 < 4;
select ID 4 because 4 >= 4. Three actual time comparisons; the first unconditional
selection is not counted as a compatibility comparison.

Why optimal? Among possible first activities, earliest finish leaves at least as
much room for the remainder as any other first choice. Replacing an optimal
schedule's first activity with this choice does not reduce the number still possible.
Repeat on the compatible remainder (the exchange argument).

`activityDP` uses a different idea:

```text
best[i] = maximum chain length ENDING at sorted position i
previous[i] = previous sorted position in that chain, or -1

best[i] = 1
for every earlier j:
    if finish[j] <= start[i] and best[j] + 1 > best[i]:
        best[i] = best[j] + 1
        previous[i] = j
```

Keep the endpoint with the largest best value. Follow previous links backwards;
fill the output array from right to left so its IDs are chronological. Strict `>`
keeps the earlier predecessor/endpoint when the length ties. Both algorithms are
deterministic, but correctness requires equal **counts**, not necessarily identical sets.

| Sorted position | Original ID | Final best | Predecessor position |
|---:|---:|---:|---:|
| 0 | 1 | 1 | -1 |
| 1 | 3 | 2 | 0 |
| 2 | 2 | 1 | -1 |
| 3 | 4 | 3 | 1 |

DP performs 0+1+2+3 = 6 compatibility tests and 3 improvements here.
It checks every earlier j, including incompatible ones. Preparation is shared and
separate. On presorted input Greedy is O(n), DP is O(n²); both return O(n) output.
The current handwritten insertion-sort preparation is O(n²), so the whole Greedy
workflow is **not** claimed to be O(n). Replacing preparation with a merge sort of
indices would improve the overall bound, but is not necessary for the small limit.

Possible change: replacing `>=` with `>` changes the problem by disallowing touching
endpoints. You must change Greedy, DP, verifier, explanations and tests together.
Adding weights would require a different objective and recurrence; this Greedy is
not a weighted-activity optimizer.

### 3. `src/trace/ActivityTrace.h`: stored history and Back

`ActivityEvent` stores the kind, sorted positions i/j, a value and compatibility flag.
`ActivityTrace` owns a growable event array and caps it at 20,000 events. On overflow,
it marks history incomplete while the solver continues. Copying the recorder is
disabled to avoid duplicate ownership of the raw allocation.

`ActivityReplay` borrows the trace and owns visible `best`, `predecessor` and
`selected` arrays. `seek(k)` resets them and reapplies events 0..k-1. Next/Back/Restart
call seek with k+1/k-1/0; targets clamp to valid bounds. This rebuild makes Back
reliable without having to reverse each kind of decision. Sorting is already done
when replay begins; preparation is identified explicitly and is not step-replayed.

`DPStart` initializes best=1. `DPUpdate` changes best/predecessor. `Select` marks an
activity during backwards reconstruction. `GreedyChoice` marks accepted candidates.
Other events explain a decision without changing the arrays. `complete()` requires
an untruncated Finished event, not merely the last stored event.

Possible question: **Why is DP's selected list empty while best values increase?**
It is calculating candidate chains. It chooses the actual answer by predecessor
reconstruction only after all rows are solved.

### 4. `src/verify/ActivityVerify.h`: feasibility versus optimality

`activityFeasible` checks ID range, no duplicate IDs, count bounds, membership and
chronological non-overlap. A valid selection of one activity is not necessarily optimal.
`activityOptimum` independently computes the optimum using a **prefix** recurrence:
OPT[k] = max(skip activity k, 1 + OPT[last compatible prefix]). This differs from the
featured DP's chain-ending-at-i recurrence. `activityVerified` requires feasibility
and equality with that optimum.

Tests go further: `bruteActivity` enumerates every subset of tiny inputs, rejects
overlapping pairs, and checks both solvers and the verifier against that independent
answer. Finite tests are evidence, not a proof covering all inputs.

### 5. `src/app/ActivityComparison.h`: coordinate a run or experiment

`runActivity` validates input, prepares order, runs one solver, and stores trace metadata.
`compareActivities` runs both solvers with counters, verifies them, then does one
warm-up and seven timed runs each. `timeActivity` does not collect traces/counters;
validation is after the clock. Both receive the same immutable input and sorted order.
Neither solver mutates them, so fresh copies are unnecessary for these solvers.

Timing excludes shared preparation, correctness checks, display, reports and destruction
of the returned result object. Solver allocations and internal-array cleanup are included.
The median is the fourth sorted sample. Zero means unresolved; a tiny timing does not
prove an algorithm is universally faster. Counts are more useful for this small demo.
The full comparison makes 18 solver calls, as with sorting.

`ActivityReport::valid` rechecks actual selected results before display/export. Its
fields describe mode, input, two results, counters, samples and trace status.

### 6. `src/io/ActivityFile.h`: persist cases and reports

```text
ALGOGENIE 1
TYPE ACTIVITY
COUNT 4
INTERVALS
1 3
2 5
3 4
4 6
END
```

`readActivityCase` parses into a temporary and replaces the destination only after
the complete file is valid. It rejects wrong type/version, malformed/out-of-range
integers, missing/extra rows, invalid intervals and trailing junk. It accepts UTF-8
BOM and whitespace. IDs are recovered from row order; the case does not save a replay.

Save/export refuses existing readable filenames and checks stream failures.
Reports contain original intervals, selected IDs, verified optimal counts, counters,
preparation count, trace status, build information and (only for Compare) seven timing
samples. CSV text is quoted using the shared formatter. Learn/Run timing is blank
with `not_measured`, not an invented zero.

Possible change: adding CSV columns requires changing header and row together and
checking an actual CSV reader, not just searching for a string.

### 7. `src/ui/ActivityViewer.h`: what the user sees

`showActivityFrame` prints the sorted input, visible DP table, selected IDs and current
event. `explainActivity` chooses explanatory text. `learnActivity` reads one letter:
N -> replay.next; B -> back; R -> restart; E -> seek(trace.size); Q -> return.
The next loop iteration redraws the reconstructed state. Lowercase works through
the existing `replayCommand`. `showActivityReport` checks validity and presents Run/Compare.

Possible change: to support F for forward, extend the N condition here and update the
help/error text. This does not change sorting controls unless its own viewer is edited.

### 8. `src/ui/ActivityMenu.h`: the complete cycle

`readActivities` handles custom input, presets and reuse of the current case.
`activityMenu` selects mode/algorithm/input, runs and checks, then remembers the case
and report before opening replay or showing results. Q leaves replay; 0 leaves the lab.

`activityCaseMenu`: successful load replaces current and clears latest; failed load
preserves both. `activityExportMenu` requires a verified latest report, then reads a path.
The functions use shared numeric/path helpers, so retry and quoted-path behaviour
are consistent with sorting. Learn accepts 20 activities; Run/Compare/files accept 100.

### 9. `src/main.cpp`: the new entry point

Three activity-state variables live outside the main loop. Main option 6 calls
`activityMenu` by reference, then continues the main loop. Existing sorting branches
retain their numbers. State belongs to the session, not to one temporary submenu call.

## Test and build evidence

- `tests/activity_checks.h`: all 1,365 ordered cases over four representative intervals
  for lengths 0..5, checked against brute subsets; replay checks for lengths 0..3;
  extreme int times, duplicate IDs, truncation, observer reset and 100 touching activities.
- `tests/activity_file_checks.h`: parser rejection/round trips, valid report contents,
  invalid-report rejection, immutable input, count and timing checks.
- `scripts/activity-test.ps1`: actual console sessions for both methods in every mode,
  navigation, current-case reuse, TXT/CSV readback, overwrite protection, boundary sizes,
  quoted paths, malformed input, EOF and switching back to sorting.
- Existing sorting tests, smoke tests and persistence tests remain in place.

Use the normal project commands:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Test
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1
powershell -ExecutionPolicy Bypass -File .\scripts\activity-test.ps1
```

Close the running executable before rebuilding. Debug and Release versions were
validated for this milestone. On this automation host, compiler subprocess DLL lookup
required a private copy of the installed compiler tools with their DLLs. This did not
change project source or the installed compiler. The supplied demo executables link
the runtime statically so they do not need MinGW DLL lookup at launch.

## Three questions to answer before presenting

1. Why does earliest finish allow an optimal solution? Give the exchange argument.
2. What does best[i] mean, and why do predecessor links reconstruct a valid chain?
3. Where does N lead? `learnActivity -> replay.next -> seek -> showActivityFrame`.

Practise the small lesson first. Do not spend tonight memorizing parser and CSV details
before you can explain those three answers. The earlier slides describe the two-solver
milestone; update their scope/status if presenting this expanded version.
