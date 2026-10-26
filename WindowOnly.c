/* Your first GUI step: create an empty window.
 * Build: gcc WindowOnly.c -o WindowOnly.exe -std=c11 -Wall -Wextra -mwindows
 * Run:   .\WindowOnly.exe
 */
#include <windows.h>

static LRESULT CALLBACK WindowProc(HWND window, UINT event,
                                   WPARAM wParam, LPARAM lParam)
{
    if (event == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(window, event, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous,
                   LPSTR commandLine, int showCommand)
{
    WNDCLASSA windowClass = {0};
    MSG message = {0};
    HWND window;
    int result;

    (void)previous;
    (void)commandLine;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "MyFirstCWindow";
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassA(&windowClass))
        return 1;

    window = CreateWindowExA(0, windowClass.lpszClassName,
                             "My first GUI in C", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
                             NULL, NULL, instance, NULL);
    if (window == NULL)
        return 1;

    ShowWindow(window, showCommand);

    while ((result = GetMessageA(&message, NULL, 0, 0)) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return result == -1 ? 1 : (int)message.wParam;
}
