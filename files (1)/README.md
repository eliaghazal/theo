# DFA Minimizer (Hopcroft) Visualizer

A C++ desktop application built with Qt 6 to visualize the creation and minimization of Deterministic Finite Automata (DFA) using Hopcroft's algorithm.

## Features

- **Visual Graph Editor**: Drag-and-drop states, create transitions, set Start/Accept states.
- **DFA Logic**: Supports custom alphabets, determinism checks, and auto-completion with sink states.
- **Hopcroft Minimization**: Step-by-step visualization of the partition refinement process.
- **Timeline**: Click through algorithm steps to see which sets are splitting.
- **JSON Support**: Import and Export DFA definitions.

## Requirements

- **C++17** compatible compiler (MSVC 2019+, GCC 9+, Clang 10+).
- **Qt 6**: Core, Gui, Widgets modules.
- **CMake**: Version 3.16 or higher.

## Build Instructions

### Linux

1.  Install dependencies:
    ```bash
    sudo apt install qt6-base-dev cmake build-essential
    ```
2.  Build:
    ```bash
    mkdir build && cd build
    cmake ..
    make
    ./DFAMinimizer
    ```

### Windows (MSVC)

1.  Install Qt 6 and CMake.
2.  Open the folder in Visual Studio or use command line:
    ```cmd
    mkdir build
    cd build
    cmake -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2019_64" ..
    cmake --build . --config Release
    ```

## Usage

1.  **Build your DFA**: Use the left panel to add states and transitions.
    -   *Add State*: Click "Add State Mode", then click on canvas.
    -   *Add Transition*: Click "Add Transition Mode", click Source, then Target. Enter symbols (e.g., `a,b`).
2.  **Validate**: Click "Validate" or "Complete w/ Sink" to ensure the DFA is ready for minimization.
3.  **Run**: Click "Run Minimization" in the right panel.
4.  **Explore**: Click on steps in the timeline to see how states were partitioned.
5.  **Toggle View**: Switch between "Original" and "Minimized" radio buttons to see the result.

## Testing

Run logic tests by executing with the `--test` flag:
```bash
./DFAMinimizer --test