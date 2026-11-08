# Sorting case and report formats

## Case TXT version 1

```text
ALGOGENIE 1
TYPE ARRAY
COUNT 3
VALUES 4 1 3
END
```

Whitespace may separate tokens across lines. An optional UTF-8 BOM is accepted.
COUNT must be 0..100, followed by exactly that many signed int values. Zero-element
cases have VALUES followed immediately by END. Trailing non-whitespace data, unsupported
versions/types, malformed integers, overflow, missing fields and overlong tokens fail.
The parser validates count before allocation and replaces the destination only after
the entire file passes. Case files contain input only, not expected answers or traces.
After loading, choose a mode and algorithm, then input option 7 to reuse the case.
Learn still limits the input to 30 elements even when a larger valid case was loaded.

## Reports

Main menu 5 exports the latest completed, verified Learn, Run or Compare calculation.
For Learn, this is the solver's final result, even if the user left replay early. The
report records whether the captured trace is complete, not whether every frame was viewed.
Reports are summaries and measurements, not saved interactive traces. Rerun a saved
case with the same algorithm to recreate its deterministic learning history.

TXT includes the input, selected algorithm/variant, result, verification, operation
counts, trace metadata, build/compiler/machine, counting rules and a reusable case block.
Copy only the ALGOGENIE..END block into a separate file if extracting a case from a report;
the full human-readable report is not itself a case file.

CSV schema version 1 has one row for Learn/Run and two rows for Compare, in Merge/Quick
order. Columns include mode, algorithm/variant, exact input/output, check status, metrics,
trace status, timing status, median and seven raw samples, build metadata and measurement
definitions. Arrays are space-separated inside quoted cells. Text uses doubled internal
quotes and quoted fields, so commas and newlines cannot break columns. Numbers use the
classic locale with a decimal point; timings retain sufficient precision for roundtrip.

Timing is collected only in Compare. Learn/Run export blank timing cells and zero
repetitions, explicitly marked not_measured. Compare exports measured or not_resolved
status; a zero median represents no resolvable elapsed ticks, not zero computational work.
The report stores the original measurements. Exporting does not run the experiment again.

## Files and session state

- Use saved/ for personal cases and reports/ for generated reports. Both folders are
  present in the repository; generated contents are ignored by Git by default.
- Examples are tracked under examples/ so a clean checkout includes reproducible inputs.
- Paths can be relative to the terminal's working folder or absolute. Spaces and paired
  surrounding double quotes are accepted. Prefer ordinary ASCII filenames with this
  older MinGW toolchain; Unicode Windows path support is not promised.
- The extension must match the chosen format (.txt or .csv). A blank path cancels.
- Existing readable files are refused rather than overwritten. Choose a fresh name.
  This local, single-user application does not promise concurrent-writer atomicity.
- Parent directories must exist. Open/write/close failures produce error messages;
  a disk failure can leave an incomplete new file, which should not be treated as valid.
- Successful loading clears the previous report. A failed load preserves current input
  and report. Running another algorithm replaces the latest report with that run's result.

## Code responsibilities

src/io/ArrayCaseFile.h: bounded tokens, validation, input serialization and file I/O.
src/models/RunReport.h: a snapshot of the latest input, verified result and measurements.
src/io/ReportFile.h: serializes this snapshot; rechecks results before allowing export.
src/ui/FileMenu.h: prompts for paths and connects file operations to session state.

Viva explanation: saving persists the problem; exporting persists evidence of a run.
They are different files because the same problem can be studied using multiple algorithms.
