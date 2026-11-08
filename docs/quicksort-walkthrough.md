# Lesson 3: Quicksort, Lomuto partition, last-element pivot

## Run it

Exit the old program first: Q from replay, then 0 from the main menu. Windows cannot
replace an executable while it is running. In the VS Code PowerShell terminal:

```powershell
cd "C:\Users\User\Desktop\Spl-1 project\AlgoGenie"
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1
.\build\algogenie.exe
```

Choose 1 (Learn), 2 (Quicksort), 2 (small input [4,1,3]).
N/B/R/E/Q followed by Enter control next/back/restart/end/menu.
The small example has 10 recorded events, plus the initial state at position 0.
P marks the pivot. B marks the partition boundary. S marks the scanned element
on comparison/keep-right frames. The screen shows state after each event.

## Trace the actual example

| Event | Array | Meaning |
| --- | --- | --- |
| Choose pivot | [4,1,3] | Last value 3 is the pivot; boundary starts at index 0 |
| Compare | [4,1,3] | Is 4 <= 3? No |
| Keep right | [4,1,3] | Leave 4 in the scanned greater-than-pivot region |
| Compare | [4,1,3] | Is 1 <= 3? Yes |
| Accept / swap | [1,4,3] | Swap indices 0 and 1; boundary advances to index 1 |
| Place pivot | [1,3,4] | Swap indices 1 and 2; pivot 3 is now final at index 1 |
| Left base case | [1,3,4] | [1] needs no further sorting |
| Right base case | [1,3,4] | [4] needs no further sorting |
| Range sorted | [1,3,4] | Both sides have returned |
| Finished | [1,3,4] | Check sorted order and preserved values |

The sides need not already be sorted after partitioning in a larger example.
Only the pivot is guaranteed to be in a valid final position at that point.

## Read src/algorithms/sorting/QuickSort.h

1. quickSort resets an optional recorder and calls quickRange for the whole array.
2. quickRange stops when end-begin < 2. The range [begin,end) excludes end.
3. partitionLast chooses values[end-1] as pivot and sets boundary=begin.
4. scan visits every item before the pivot. If values[scan] <= pivot, swap it into
   the boundary position and increment boundary. Otherwise leave boundary unchanged.
5. After scanning, swap the pivot into boundary and return that index.
6. Recurse on [begin,pivot) and [pivot+1,end), excluding the settled pivot.

Inside partitionLast, initially ignore record(...) to see the algorithm itself.
Then read those calls as a log of its decisions. No input/output code lives in the solver.

## The partition invariant

Before each comparison:

```text
[begin, boundary)   values <= pivot
[boundary, scan)    values > pivot
[scan, end-1)       not yet examined
end-1              pivot position
```

When a small-enough value is found, the swap extends the <= region by one. A larger
value extends the scanned > region. At loop exit, swapping the pivot into boundary
puts it between these regions. The partition workspace is constant; recursion still
uses stack space.

## Replay design

We reuse SortTrace, SortReplay and SortViewer. PartitionSwap and PivotPlaced events
store the two indices needed to reproduce a swap. Self-swaps leave values unchanged;
the acceptance event still explains the boundary advance. Current-event fields hold
pivot value/index and boundary, so stepping back restores these displayed labels too.
ComparePivot and KeepRight explain decisions without modifying array data.

Replay reconstruction starts from the original array. It does not call quickSort again.
Quicksort does not use the merge buffer, so that row is hidden in its viewer.

## Viva questions

- Why last-element pivot? It is a simple deterministic baseline for explanation and
  repeatable lessons. It also exposes bad partitions on sorted input.
- Why exclude the pivot from recursive calls? Partitioning has already positioned it;
  excluding it reduces the ranges and avoids repeatedly processing the same pivot.
- Is Quicksort stable? This swap-based version is not stable: equal-key items can
  change relative order. Choosing <= does not make Quicksort stable.
- Is it always O(n log n)? No. With distinct keys in random order, the usual average
  is O(n log n); sorted, reverse-sorted or all-equal inputs cause O(n^2) here.
- How much space? Partition itself O(1); average recursion depth O(log n) under
  balanced/random-order behaviour, worst O(n). Trace and original-copy storage are extra.
- Why two checks? Sortedness does not alone guarantee no values were lost or duplicated.
- Why not randomized pivot now? We selected an explainable fixed-pivot baseline.
  A future randomized variant needs recorded choices or a seed for reproducible lessons.

## Tests and limits

Console Learn input is capped at 30 elements, Run at 100. Events are capped at 20,000;
overflow stops recording but allows sorting to finish. An incomplete trace is labelled.

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Test
powershell -ExecutionPolicy Bypass -File .\scripts\smoke-test.ps1
```

Tests check known answers, integer limits, all short arrays over {-1,0,1}, the partition
invariant, pivot placement, deterministic replay, every-frame value preservation,
forward/back/restart, recorder overflow, sorted/reversed/equal inputs, and both menus.

Compare mode has now been added: see comparison-walkthrough.md for identical original
inputs, documented operation counts and separate untraced timing. Saved cases and export
are still pending.
