# Lesson 2: understand Merge Sort and recorded replay

## Run this exact lesson

In the VS Code PowerShell terminal:

```powershell
cd "C:\Users\User\Desktop\Spl-1 project\AlgoGenie"
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1
.\build\algogenie.exe
```

Enter 1 for Learn, then 1 for Merge Sort, then 2 for [4,1,3]. Each command needs Enter.
N: next; B: back; R: restart; E: end; Q: return to menu. At the main menu, 0 exits.
The example has 23 recorded events. Step zero is the original input.
Use N several times, B once, N again, R, then E. At the end expect [1,3,4]
and two PASS labels. The buffer is separate temporary storage; dots hide unused cells.

## Read the algorithm first

Open src/algorithms/sorting/MergeSort.h. Ignore the record(...) calls on the first read.

1. mergeSort creates one temporary buffer and calls mergeRange on the whole array.
2. begin is inclusive, end is exclusive. [0,3) contains indices 0,1,2.
   The number of elements in that range is end-begin.
3. If end-begin < 2, return: an empty or one-element range is already sorted.
4. middle = begin + (end-begin)/2 divides the range. Integer division rounds down.
5. Recursively sort [begin,middle), then [middle,end).
6. left and right point at the next unused items in the sorted halves. next points
   at the next buffer slot. Copy the smaller item, then advance its pointer.
7. <= chooses the left item when values tie, preserving their original relative order.
8. When one half runs out, copy the remaining items from the other.
9. Copy the completed buffer range back into the main array.

Hand trace of [4,1,3]:

```text
[4,1,3]
  left: [4]         already sorted
  right: [1,3]
    [1] and [3]     each already sorted
    merge -> [1,3]
  merge [4] with [1,3]
    choose 1 -> choose 3 -> copy remaining 4
  result: [1,3,4]
```

The merge invariant: the written buffer prefix is sorted and contains the smallest
elements consumed so far from the two sorted halves.

## Understand the C++ used here

- IntArray& means the function uses the caller's array, so writes change it.
- const IntArray& allows reading an array without modifying it.
- SortTrace* trace is an optional pointer to a recorder; nullptr means no recorder.
- enum class gives event kinds readable names, such as Compare and CopyBack.
- A struct groups the fields of one event together.
- new[] allocates owned array storage; delete[] releases it in the destructor.
- IntArray result(original) makes an independent copy. Sorting it preserves original.
- inline permits these small header-defined functions to be included in multiple
  translation units; it does not guarantee compiler inlining.

## Then read the replay code

src/trace/SortTrace.h stores events in a handwritten growable array. Each event
describes an action, active range, recursion depth and relevant indices/value.
Capacity grows when necessary. The 20,000-event limit bounds recorded history;
overflow stops recording, not the solver, and is labelled incomplete.

src/trace/SortReplay.h starts with an independent copy of the initial array.
seek(k) restores the input, resets buffer display state, and applies the first k events.
BufferWrite changes the displayed buffer. CopyBack changes the displayed array.
The other event types explain decisions without changing array values.
BeginMerge hides stale buffer cells from previous merges.

Next seeks to k+1. Back seeks to k-1. Restart seeks to zero. The sorting function is
not called by these controls. The current event supplies the range and depth labels.
This implementation shows the current recursion depth, not a complete call-stack panel.

src/ui/SortViewer.h turns that state into a terminal screen and handles commands.
src/main.cpp connects input, execution, verification and the selected mode.

## Six viva questions to practise aloud

1. Why does recursion stop? A range with fewer than two items is already sorted;
   every split makes smaller ranges, so we eventually reach that case.
2. Why use a buffer? Writing merged values directly over the input could overwrite
   values in the halves that have not yet been consumed.
3. Why choose the left on equality? It preserves the order of equal-key elements.
4. Why O(n log n)? There are logarithmically many split levels and linear total
   merging work per level. The shared buffer uses O(n) space, recursion O(log n).
5. Does Back undo sorting? No. The completed run was recorded; Back reconstructs
   an earlier display state by applying a shorter prefix of its events.
6. Why two result checks? Sorted order alone could accept a sorted but corrupted
   array. Frequency checking also confirms the original values and multiplicities.

Be precise about overhead: O(n log n) describes the solver, excluding tracing,
verification and rendering. Verification here is O(n^2). Stored trace space is O(E)
for E recorded events, capped at 20,000. seek(k) costs O(n+k+n*m), where m is the
number of BeginMerge events in that prefix: each clears n buffer-mask entries.
This simple implementation favours understandable reconstruction over speed. Small learning
inputs are capped at 30 elements; result-only mode accepts up to 100.

## Tests

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Test
powershell -ExecutionPolicy Bypass -File .\scripts\smoke-test.ps1
```

Checks cover result correctness, deterministic events, copy-back consistency,
left selection on equal keys, next/back restoration, restart, end boundaries,
truncation and recorder reuse. Console checks cover controls, invalid input,
empty-array learning and returning from learning to result-only mode.

## Explain before the next feature

For [4,1,3], explain why the first split is [4] and [1,3], why the first comparison
is 1 versus 3, and why the original array does not change on a Compare event.
Then demonstrate Back and explain the initial-state-plus-events design.
