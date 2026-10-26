/*
 * Guess the Number - a Windows desktop game written in C.
 * Build in PowerShell, from this file's folder:
 * gcc GuessGUI.c -o GuessGUI.exe -std=c11 -Wall -Wextra -mwindows -lgdi32
 * Run: .\GuessGUI.exe
 * Read START_HERE.txt for the beginner walkthrough.
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>

#define ID_INPUT 101
#define ID_GUESS 102
#define ID_RESTART 103

/* These are the same three variables as in your console game. */
static int randomNumber;
static int no_of_guesses;
static int gameWon;

/* HWND is a handle: Windows' way of identifying a window or control. */
static HWND inputBox, guessButton, restartButton, hintLabel, countLabel;
static HFONT bodyFont, titleFont;
static HBRUSH backgroundBrush;
static HINSTANCE appInstance;
static int dpi = 96;
static COLORREF hintColor;

/* Scale positions and fonts for Windows display scaling. */
static int pixels(int value)
{
    return MulDiv(value, dpi, 96);
}

static HWND addControl(HWND parent, const char *type, const char *text,
                       DWORD style, int id, int x, int y, int w, int h)
{
    HWND control = CreateWindowExA(
        0, type, text, WS_CHILD | WS_VISIBLE | style,
        pixels(x), pixels(y), pixels(w), pixels(h), parent,
        (HMENU)(INT_PTR)id, appInstance, NULL);

    if (control != NULL)
        SendMessageA(control, WM_SETFONT, (WPARAM)bodyFont, TRUE);

    return control;
}

static void showHint(const char *text, COLORREF color)
{
    hintColor = color;
    SetWindowTextA(hintLabel, text);
    InvalidateRect(hintLabel, NULL, TRUE);
}

/* NEW GAME: choose one secret number, then reset the interface. */
static void startNewGame(void)
{
    randomNumber = (rand() % 100) + 1;
    no_of_guesses = 0;
    gameWon = 0;

    SetWindowTextA(inputBox, "");
    SetWindowTextA(countLabel, "Guesses: 0");
    showHint("Your first guess starts the game.", RGB(65, 79, 100));
    EnableWindow(inputBox, TRUE);
    EnableWindow(guessButton, TRUE);
    SetFocus(inputBox);
}

/* GUESS: called once per button click or Enter key press. */
static void checkGuess(void)
{
    char input[32];
    char message[160];
    char *end;
    long guessed_number;

    if (gameWon)
        return;

    /* This replaces scanf: read the text from the input box. */
    GetWindowTextA(inputBox, input, (int)sizeof(input));
    errno = 0;
    guessed_number = strtol(input, &end, 10);

    if (end == input)
    {
        showHint("Enter a whole number from 1 to 100.", RGB(165, 40, 40));
        SetFocus(inputBox);
        return;
    }

    while (isspace((unsigned char)*end))
        end++;

    if (errno == ERANGE || *end != '\0' ||
        guessed_number < 1 || guessed_number > 100)
    {
        showHint("Enter a whole number from 1 to 100.", RGB(165, 40, 40));
        SetFocus(inputBox);
        SendMessageA(inputBox, EM_SETSEL, 0, (LPARAM)-1);
        return; /* Invalid input does not use a guess. */
    }

    no_of_guesses++;
    snprintf(message, sizeof(message), "Guesses: %d", no_of_guesses);
    SetWindowTextA(countLabel, message);

    /* The comparison logic is the same as your original C program. */
    if (guessed_number > randomNumber)
    {
        showHint("Too high! Try a lower number.", RGB(29, 83, 155));
    }
    else if (guessed_number < randomNumber)
    {
        showHint("Too low! Try a higher number.", RGB(29, 83, 155));
    }
    else
    {
        snprintf(message, sizeof(message),
                 "Correct! The number was %d.\r\nYou won in %d guess%s!",
                 randomNumber, no_of_guesses, no_of_guesses == 1 ? "" : "es");
        showHint(message, RGB(20, 113, 67));
        gameWon = 1;
        EnableWindow(inputBox, FALSE);
        EnableWindow(guessButton, FALSE);
        SetFocus(restartButton);
        return;
    }

    /* Select the previous guess so typing replaces it. */
    SetFocus(inputBox);
    SendMessageA(inputBox, EM_SETSEL, 0, (LPARAM)-1);
}

/* Windows calls this function when something happens in our window. */
static LRESULT CALLBACK WindowProc(HWND window, UINT event,
                                   WPARAM wParam, LPARAM lParam)
{
    switch (event)
    {
        case WM_CREATE:
        {
            HWND heading = addControl(window, "STATIC", "Guess the number",
                                      SS_LEFT, 0, 28, 26, 364, 44);
            SendMessageA(heading, WM_SETFONT, (WPARAM)titleFont, TRUE);
            addControl(window, "STATIC", "A little mystery. One number. Your move.",
                       SS_LEFT, 0, 28, 76, 364, 30);
            addControl(window, "STATIC", "I'm thinking of a number from 1 to 100.",
                       SS_LEFT, 0, 28, 124, 364, 25);
            addControl(window, "STATIC", "Your guess", SS_LEFT,
                       0, 28, 166, 364, 24);

            inputBox = addControl(window, "EDIT", "",
                                  WS_BORDER | WS_TABSTOP | ES_NUMBER |
                                  ES_AUTOHSCROLL, ID_INPUT, 28, 194, 224, 38);
            SendMessageA(inputBox, EM_SETLIMITTEXT, 8, 0);
            guessButton = addControl(window, "BUTTON", "&Guess",
                                     WS_TABSTOP | BS_DEFPUSHBUTTON,
                                     ID_GUESS, 264, 194, 128, 38);
            hintLabel = addControl(window, "STATIC", "", SS_LEFT,
                                   0, 28, 252, 364, 58);
            countLabel = addControl(window, "STATIC", "Guesses: 0", SS_LEFT,
                                    0, 28, 328, 180, 28);
            restartButton = addControl(window, "BUTTON", "&New game",
                                       WS_TABSTOP | BS_PUSHBUTTON,
                                       ID_RESTART, 264, 322, 128, 38);
            addControl(window, "STATIC", "Enter to guess  |  Tab to move between controls",
                       SS_LEFT, 0, 28, 386, 364, 38);

            if (!heading || !inputBox || !guessButton || !hintLabel ||
                !countLabel || !restartButton)
                return -1;

            startNewGame();
            return 0;
        }

        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                if (LOWORD(wParam) == ID_GUESS)
                    checkGuess();
                else if (LOWORD(wParam) == ID_RESTART)
                    startNewGame();
            }
            return 0;

        case WM_CTLCOLORSTATIC:
        {
            HDC dc = (HDC)wParam;
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, (HWND)lParam == hintLabel
                             ? hintColor : RGB(31, 45, 65));
            return (LRESULT)backgroundBrush;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcA(window, event, wParam, lParam);
}

/* WinMain is the entry point for this Windows GUI application. */
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous,
                   LPSTR commandLine, int showCommand)
{
    const char className[] = "GuessNumberGameWindow";
    WNDCLASSA windowClass = {0};
    RECT size = {0, 0, 0, 0};
    MSG message = {0};
    HWND window;
    HDC screen;
    int messageResult;
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    (void)previous;
    (void)commandLine;
    appInstance = instance;
    srand((unsigned int)time(NULL)); /* Seed once, outside the game logic. */

    SetProcessDPIAware();
    screen = GetDC(NULL);
    if (screen != NULL)
    {
        dpi = GetDeviceCaps(screen, LOGPIXELSX);
        ReleaseDC(NULL, screen);
    }

    backgroundBrush = CreateSolidBrush(RGB(245, 248, 252));
    bodyFont = CreateFontA(-MulDiv(11, dpi, 72), 0, 0, 0, FW_NORMAL,
                           FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");
    titleFont = CreateFontA(-MulDiv(22, dpi, 72), 0, 0, 0, FW_BOLD,
                            FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hbrBackground = backgroundBrush;

    if (!RegisterClassA(&windowClass))
    {
        MessageBoxA(NULL, "Could not register the window.", "Startup error", MB_ICONERROR);
        DeleteObject(bodyFont);
        DeleteObject(titleFont);
        DeleteObject(backgroundBrush);
        return 1;
    }

    size.right = pixels(420);
    size.bottom = pixels(438);
    AdjustWindowRect(&size, style, FALSE);
    window = CreateWindowExA(0, className, "Guess the Number", style,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             size.right - size.left, size.bottom - size.top,
                             NULL, NULL, instance, NULL);

    if (window == NULL)
    {
        MessageBoxA(NULL, "Could not create the game window.", "Startup error", MB_ICONERROR);
        UnregisterClassA(className, instance);
        DeleteObject(bodyFont);
        DeleteObject(titleFont);
        DeleteObject(backgroundBrush);
        return 1;
    }

    ShowWindow(window, showCommand);
    UpdateWindow(window);
    SetFocus(inputBox);

    /* This message loop keeps the interface responsive. It does not
       repeatedly ask for guesses like the old do-while loop did. */
    while ((messageResult = GetMessageA(&message, NULL, 0, 0)) > 0)
    {
        if (message.message == WM_KEYDOWN && message.wParam == VK_RETURN)
        {
            if (gameWon || GetFocus() == restartButton)
                startNewGame();
            else
                checkGuess();
            continue;
        }

        if (!IsDialogMessageA(window, &message))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }

    if (IsWindow(window))
        DestroyWindow(window);
    UnregisterClassA(className, instance);
    DeleteObject(bodyFont);
    DeleteObject(titleFont);
    DeleteObject(backgroundBrush);
    return messageResult == -1 ? 1 : (int)message.wParam;
}
