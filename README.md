# Project 1: Number Guessing Game in C

A beginner C project that turns a console number-guessing game into a Windows desktop application using the Win32 API.

The computer chooses a secret number between **1 and 100**. Enter guesses, follow the higher/lower hints, and find the number in as few attempts as possible.

## Features

- A random secret number from 1 to 100 for each new game.
- A text box and **Guess** button for submitting guesses.
- Higher/lower hints after each incorrect guess.
- A counter that includes the winning guess.
- Input validation: empty or invalid input does not increase the counter.
- A success message showing the secret number and total guesses.
- A **New game** button that resets the game.
- Keyboard support for Enter, Tab, and Space.

## Built With

| Tool | Purpose |
| --- | --- |
| C | Game logic and application code |
| Win32 API | Window, text box, buttons, and event handling |
| MinGW-w64 GCC | Compiling the Windows executable |
| VS Code | Editing the source code |
| PowerShell | Running the build commands |

## Project Files

| File | Purpose |
| --- | --- |
| `GuessGUI.c` | Complete graphical number-guessing game |
| `WindowOnly.c` | Small starter program that creates an empty window |
| `START_HERE.txt` | Detailed beginner walkthrough and learning exercises |
| `README.md` | Project overview and setup instructions |

## Requirements

- Windows.
- A Windows MinGW-w64 GCC compiler, such as MSYS2 UCRT64 GCC.
- GCC available in your terminal's `PATH`.
- A code editor; the instructions below use VS Code.

Check that GCC is available:

```powershell
gcc --version
```

## Build and Run

### 1. Open the project folder

In VS Code, choose **File → Open Folder** and select the folder containing `GuessGUI.c`.

Choose **Terminal → New Terminal** and use PowerShell. The terminal must be in the same folder as the source files. Run `dir` to check the filenames.

### 2. Compile the game

```powershell
gcc GuessGUI.c -o GuessGUI.exe -std=c11 -Wall -Wextra -mwindows -lgdi32
```

### 3. Run the game

```powershell
.\GuessGUI.exe
```

After editing the source, save the file, close the running game, and compile again before running it.

### Optional: Start with an empty window

To learn the basic window structure first, compile the smaller example:

```powershell
gcc WindowOnly.c -o WindowOnly.exe -std=c11 -Wall -Wextra -mwindows
```

Then run it:

```powershell
.\WindowOnly.exe
```

Compile these programs separately. Both files define their own `WinMain`, so a command such as `gcc *.c` would combine two application entry points.

### Compiler Flags

| Flag | Meaning |
| --- | --- |
| `-o GuessGUI.exe` | Names the output executable |
| `-std=c11` | Selects the C11 language standard |
| `-Wall -Wextra` | Enables useful compiler warnings |
| `-mwindows` | Builds a Windows GUI application |
| `-lgdi32` | Links the graphics functions used for fonts and colors |

## How to Play

1. Enter a whole number between **1 and 100**.
2. Click **Guess**, or press **Enter** while the input box has focus.
3. Follow the hint: choose a higher or lower number.
4. Continue until the success message appears.
5. Click **New game** to choose another secret number and reset the counter.

Each valid submission counts as one guess, including repeated guesses and the winning guess. Invalid input does not count. There is no attempt limit.

For example, if the secret number were **42**:

| Guess | Feedback | Total guesses |
| --- | --- | --- |
| 50 | Too high; try a lower number | 1 |
| 30 | Too low; try a higher number | 2 |
| 42 | Correct | 3 |

### Keyboard Controls

| Key | Action |
| --- | --- |
| Enter | Submits a guess when the input box has focus |
| Enter after winning | Starts a new game |
| Enter on the New game button | Starts a new game |
| Tab / Shift + Tab | Moves between controls |
| Space | Activates a focused button |

## How the Program Works

The random number generator is seeded once when the application starts. Each new game chooses one secret number and keeps it unchanged until the game is restarted.

The GUI uses **events**. When the player clicks **Guess**, Windows sends a message to the application. The application reads the input, validates it, increments the counter, compares it with the secret, and updates the hint.

The Windows message loop keeps the interface responsive between guesses. Each click processes one guess; the original console `scanf` and guessing `do-while` loop are replaced by GUI input and event handling.

| Console version | GUI version |
| --- | --- |
| `scanf()` | `GetWindowTextA()` reads the input box |
| `printf()` for feedback | `SetWindowTextA()` updates a label |
| Repeated input in a `do-while` loop | A button event calls `checkGuess()` |
| `main()` | `WinMain()` starts this Windows application |

### Main Functions

| Function | Responsibility |
| --- | --- |
| `startNewGame()` | Chooses a secret and resets the game state and controls |
| `checkGuess()` | Reads and validates a guess, counts it, and displays feedback |
| `showHint()` | Updates the hint text and its color |
| `addControl()` | Creates a label, text box, or button |
| `pixels()` | Scales control positions for the Windows display setting |
| `WindowProc()` | Handles window creation, button clicks, and other messages |
| `WinMain()` | Sets up the application and runs the message loop |

## What This Project Teaches

- Variables, comparisons, and `if-else` statements.
- Random numbers with `rand()` and `srand()`.
- Splitting a program into functions.
- Converting input text to numbers using `strtol()`.
- Keeping game state between user actions.
- Creating a basic Windows GUI and handling events.
- Compiling and linking a C application with GCC.

For a useful first exercise, temporarily replace the random assignment in `startNewGame()` with `randomNumber = 42;`. Try guesses of 20, 80, and 42, then restore random number generation.

## Troubleshooting

| Problem | What to check |
| --- | --- |
| `gcc` is not recognized | Check the GCC installation and `PATH`, then reopen VS Code |
| Source file not found | Open the terminal in the source folder and check the `.c` extension |
| `windows.h` is missing | Use Windows MinGW-w64 GCC; WSL's Linux GCC targets Linux |
| Undefined graphics references | Keep `-lgdi32` at the end of the game build command |
| Permission denied when rebuilding | Close the running executable before compiling again |
| Multiple definitions of `WinMain` | Compile `GuessGUI.c` and `WindowOnly.c` separately |

## Current Status

The GUI source and starter example are included. Windows compilation and visual testing have not yet been completed in the development environment used to prepare this project.

This implementation targets Windows. Scores are not saved between games or application launches.

## Possible Future Improvements

- Difficulty levels with different number ranges.
- An optional limit on the number of guesses.
- A visible guess history.
- A saved best score.

## Learning Reference

[Microsoft Learn: Your First Windows Program](https://learn.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program)

The Microsoft tutorial uses C++; this project's source files use C.
