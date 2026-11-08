# Spl-1
AlgoGenie

## Working milestone - 8 November 2026

Merge Sort and last-element-pivot Quicksort run with validated input, examples,
result checks and learning replay. Inspect merge buffers or pivot/partition/swapping
decisions with next/back/restart/end controls. Comparison, saved cases and reports
are **not yet implemented**.

Open a PowerShell terminal in this folder:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1
.\build\algogenie.exe
```

Choose `1` for Learn, then algorithm `1` (Merge Sort) or `2` (Quicksort),
then input `2` for the small `[4,1,3]` lesson.
Use `N`, `B`, `R`, `E`, `Q` followed by Enter. Choose `2` from the main menu for result-only mode.
To run tests:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Test
powershell -ExecutionPolicy Bypass -File .\scripts\smoke-test.ps1
```

See [Learning-mode walkthrough](docs/learning-mode-walkthrough.md) for the code reading order,
hand trace and viva questions, [Quicksort walkthrough](docs/quicksort-walkthrough.md),
and [November 12 preparation plan](docs/presentation-plan.md).
The original proposal below
describes the full planned application.

See How an Algorithm Makes Its Decisions.

An Offline C++ Algorithm Learning and Experimentation Tool

goGenie is an offline C++17 application designed to help students understand how algorithms work internally through step-by-step execution replay, solution verification, and algorithm comparison.

Rather than simply displaying the final output, AlgoGenie aims to show the decisions an algorithm makes, how its internal state changes, and why a particular result is produced.

Users will be able to enter their own problem inputs, execute different algorithms, inspect their execution steps, verify solutions, and compare the approaches.

The project focuses on five major problem categories and ten algorithm implementations, covering sorting, graph algorithms, shortest paths, coin change, and activity selection.

«Project Status: 🚧 Under Development — This repository represents an academic SPL-1 project. Features and algorithms described below are planned unless explicitly marked as implemented.»

---

🎯 Why AlgoGenie?

Students often learn algorithms by studying pseudocode, complexity analysis, and final outputs. However, understanding why an algorithm makes a particular decision can be challenging.

AlgoGenie aims to bridge this gap by providing two main modes:

1. Learn Mode

Understand how an algorithm works by following its execution.

- Observe algorithmic decisions step by step.
- Inspect intermediate states.
- Move forward and backward through execution history.
- Restart and replay an algorithm.
- Understand why particular operations or decisions occur.
- Verify the final solution.

2. Compare Mode

Explore different approaches to solving the same problem.

- Execute two suitable algorithms using the same input.
- Compare their outputs.
- Verify the correctness of both solutions.
- Compare operation counts and other relevant metrics.
- Understand the differences between algorithmic approaches.

---

📚 Algorithms and Problem Categories

AlgoGenie will feature 10 algorithm implementations across 5 problem categories.

1. Sorting Algorithms

<text>Input: Integer array</text>

Algorithm| Paradigm| Expected Result
Merge Sort| Divide and Conquer| Sorted array
Quick Sort| Divide and Conquer| Sorted array

Users will be able to inspect array divisions, comparisons, partitions, and merging operations.

2. Minimum Spanning Tree

<text>Input: Weighted, undirected graph</text>

Algorithm| Paradigm| Expected Result
Prim's Algorithm| Greedy| Minimum Spanning Tree
Kruskal's Algorithm| Greedy| Minimum Spanning Tree

This category demonstrates how different greedy strategies construct a minimum spanning tree.

3. Shortest Path Algorithms

<text>Input: Weighted graph and source vertex</text>

Algorithm| Paradigm| Expected Result
Dijkstra's Algorithm| Greedy| Shortest distances and paths
Bellman-Ford Algorithm| Edge Relaxation| Shortest distances and paths

The implementations will account for their different constraints:

- Dijkstra's algorithm requires nonnegative edge weights.
- Bellman-Ford supports negative edge weights and detects reachable negative-weight cycles.

4. Coin Change

<text>Input: Coin denominations and target amount</text>

Algorithm| Paradigm| Expected Result
Greedy Coin Change| Greedy| Coin selection and count
Dynamic Programming| Dynamic Programming| Optimal coin selection and count

This category demonstrates the difference between greedy selection and optimal substructure using dynamic programming.

5. Activity Selection

<text>Input: Activities with start and finish times</text>

Algorithm| Paradigm| Expected Result
Earliest-Finish-Time Greedy| Greedy| Maximum compatible activity set
Dynamic Programming| Dynamic Programming| Maximum compatible activity set

The objective is to select the maximum number of mutually compatible activities without overlapping.

This category will demonstrate two different approaches to solving the same optimization problem.

---

⚙️ How the Application Works

The planned application follows this workflow:

             MAIN MENU
                 |
       +---------+---------+
       |                   |
    LEARN                COMPARE
       |                   |
       +---------+---------+
                 |
          CHOOSE PROBLEM
                 |
       Array / Graph / Coins
          / Activities
                 |
                 v
          ENTER INPUT
                 |
                 v
        INPUT VALIDATION
                 |
                 v
        SELECT ALGORITHM
                 |
                 v
        ALGORITHM EXECUTION
                 |
                 v
           TRACE RECORDER
                 |
        +--------+--------+
        |                 |
        v                 v
    STEP REPLAY      COMPARISON
        |                 |
        +--------+--------+
                 |
                 v
       RESULT VERIFICATION
                 |
                 v
          DISPLAY RESULT
                 |
          +------+------+
          |             |
        SAVE          EXPORT
        CASE          REPORT

Main Menu

The proposed terminal interface will provide the following options:

==================================
           ALGOGENIE
==================================

1. Learn
2. Compare
3. Load or Save a Case
4. Export Latest Report
0. Exit

Choose an option:

Users will then select a problem category, provide input, and choose the appropriate algorithm or algorithms.

---

🔍 Example: Comparing Greedy and DP Activity Selection

Consider the following activities:

Activity| Start| Finish
A| 1| 3
B| 2| 5
C| 3| 4
D| 4| 6

The objective is to select the maximum number of non-overlapping activities.

Expected comparison:

Method| Selected Activities| Count
Greedy| A, C, D| 3
Dynamic Programming| A, C, D| 3

Both algorithms produce a valid maximum-size selection.

AlgoGenie will allow users to inspect how each method reaches its result, verify that the selected activities do not overlap, and compare their computational work.

This is an illustrative example from the project proposal, not a claim of implemented functionality.

---

🔄 Step-by-Step Execution Replay

One of AlgoGenie's central features is its execution-tracing mechanism.

For example, in Greedy Activity Selection, the application will record events such as:

Step| Decision| Selected So Far
1| Select A (1, 3)| A
2| Select C (3, 4)| A, C
3| Reject B (2, 5): overlaps| A, C
4| Select D (4, 6)| A, C, D

Users will be able to navigate through the recorded events using:

- "N" — Next step
- "B" — Previous step
- "R" — Restart
- "Q" — Return to menu

The purpose is to make algorithmic decisions understandable rather than displaying only the final answer.

---

🧩 Core Components

The application will be developed using a modular architecture.

Component| Responsibility
Input Validation| Validate problem inputs and constraints
Data Models| Represent arrays, graphs, coins, and activities
Algorithm Engine| Execute the ten algorithm implementations
Trace Recorder| Record important algorithmic events
Replay System| Navigate through recorded execution steps
Verification Module| Check the validity of algorithm outputs
Metrics Collector| Record algorithm-specific operation counts and timing
File Handling| Save and load problem cases
Report Generator| Export experiment results to CSV
Terminal Interface| Provide menus, inputs, and output displays

Custom Data Structures

The project will emphasize handwritten data structures, including:

- Custom Dynamic Array
- Graph representation using adjacency lists and edge lists
- Indexed Min Heap
- Disjoint Set Union (DSU)
- Trace Event storage

These structures will support the implementation of the algorithms without relying on external libraries.

---

📊 Performance Comparison

AlgoGenie will provide a way to examine how different algorithms perform on the same input.

Planned measurements include:

- Number of comparisons.
- Number of writes or updates.
- Algorithm-specific operation counts.
- Execution time.
- Correctness and validity of the final result.

Timing will be measured separately from traced execution so that the overhead of recording events does not distort performance comparisons.

---

💾 File Handling and Reports

The application is planned to support:

- Saving problem cases in TXT format.
- Loading previously saved cases.
- Reusing inputs for further experiments.
- Exporting the latest comparison or experiment report in CSV format.

This will make it possible to reproduce experiments and examine their results outside the application.

---

🛠️ Technology Stack

Technology| Purpose
C++17| Core programming language
GCC / G++| Compilation
VS Code| Development environment
Git| Version control
GitHub| Source code and progress tracking
TXT| Problem-case storage
CSV| Experiment report export

Development approach: Offline-first, terminal-based application.

The proposed academic baseline does not use external libraries. Algorithms and required data structures will be implemented manually using permitted C++ facilities.

---

📁 Planned Repository Structure

AlgoGenie/
│
├── src/
│   ├── main.cpp
│   │
│   ├── app/
│   │
│   ├── core/
│   │   ├── DynamicArray.h
│   │   ├── IndexedMinHeap.h
│   │   └── DisjointSet.h
│   │
│   ├── models/
│   │
│   ├── algorithms/
│   │   ├── sorting/
│   │   ├── mst/
│   │   ├── shortest_path/
│   │   ├── coin_change/
│   │   └── activity_selection/
│   │
│   ├── trace/
│   │
│   ├── verify/
│   │
│   ├── io/
│   │
│   └── ui/
│
├── tests/
│
├── examples/
│
├── docs/
│
├── .gitignore
└── README.md

The structure is planned and will evolve as development progresses.

---

🗺️ Development Roadmap

Stage| Planned Deliverables
Weeks 1–3| Coin Change, Merge Sort, and basic execution replay
Weeks 4–7| Activity Selection, Quick Sort, and graph algorithms
Weeks 8–11| Verification, benchmarking, TXT and CSV functionality
Weeks 12–14| Testing, documentation, integration, and final demonstration

The development process will focus on building, testing, and documenting working components incrementally.

---

🧪 Testing and Verification

Correctness is an essential part of AlgoGenie.

Planned testing will include:

- Normal and boundary inputs.
- Duplicate array elements.
- Invalid problem inputs.
- Disconnected graphs.
- Negative edge weights where applicable.
- Unreachable graph vertices.
- Reachable negative-weight cycles.
- Coin-change cases where Greedy does not produce an optimal solution.
- Activity-selection cases involving overlapping activities.

Each algorithm will be checked against the expected properties of its corresponding problem.

---

🎓 Project Information

Detail| Information
Project| AlgoGenie
Course| SPL-1
Institution| Institute of Information Technology
University| University of Dhaka
Developer| Md. Sajid Mahmud
Student ID| BSSE 1738
Language| C++17
Project Type| Offline Algorithm Learning Tool

---

🌱 Our Vision

AlgoGenie is not simply intended to be a collection of algorithm implementations.

Its main purpose is to help students understand the reasoning and decisions behind algorithms.

Instead of only seeing:

"Input → Algorithm → Output"

users should be able to explore:

"Input → Decisions → Intermediate States → Execution Replay → Verified Result"

By combining algorithm learning, execution tracing, solution verification, and performance comparison, AlgoGenie aims to provide a practical environment for exploring fundamental algorithmic concepts.

AlgoGenie — See how an algorithm makes its decisions.
