// HangmanGame.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "App.h"
#include "Hangman.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
Hangman hangman;								// Hangman game instance
wchar_t buffer[MAX_LOADSTRING] = L"HANGMAN";    // Buffer for new word //bad
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    NewWordDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void                DrawHangman(HDC hdc, int wrongGuesses);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HANGMANGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HANGMANGAME));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HANGMANGAME));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_HANGMANGAME);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    hangman.Initialize(buffer);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_NEW_WORD: // This is the command ID defined in the resource file
        {
            // When calling DialogBoxParam
            if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_NEW_WORD_DIALOG), hWnd, NewWordDialogProc, lParam) == IDOK)
            {
                // buffer is a global variable because i was not able to transport it with lParam
                hangman.Initialize(buffer);
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Drawing the guessed word
        std::wstring displayWord = hangman.GetDisplayWord();
        TextOut(hdc, 10, 10, displayWord.c_str(), static_cast<int>(displayWord.length()));

        // Drawing guessed characters
        std::wstring guessedCharacters = hangman.GetGuessedCharacters();
        TextOut(hdc, 10, 30, guessedCharacters.c_str(), static_cast<int>(guessedCharacters.length()));

        // Drawing hangman based on the number of wrong guesses
        DrawHangman(hdc, hangman.GetWrongGuessCount());

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_KEYDOWN:
    {
        wchar_t guessedChar = static_cast<wchar_t>(wParam);
        if (hangman.GetGuessedCharacters().find(guessedChar) != std::wstring::npos) {
            break;
        }
        hangman.AddGuessedCharacters(guessedChar);
        hangman.Guess(guessedChar);

        // Check for win/loss condition
        if (hangman.IsGameOver()) {
            MessageBox(hWnd, L"You lost!", L"Game Over", MB_OK);
			// Reset game
			hangman.Initialize(buffer);
        }
        else {
            if (hangman.IsGameWon()) {
                MessageBox(hWnd, L"You won!", L"Congratulations", MB_OK);
                // Reset game
                hangman.Initialize(buffer);
            }
        }

        InvalidateRect(hWnd, NULL, TRUE);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD
(wParam) == IDCANCEL)
{
    EndDialog(hDlg, LOWORD(wParam));
    return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK NewWordDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            GetDlgItemText(hDlg, IDC_NEW_WORD_EDIT, buffer, MAX_LOADSTRING);
            CharUpperBuff(buffer, lstrlen(buffer));
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)FALSE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void DrawHangman(HDC hdc, int wrongGuesses)
{
    // Base coordinates for drawing the hangman
    int baseX = 200;
    int baseY = 300; // Adjusted for better position
    int headRadius = 20;
    int bodyLength = 60; // Length of the body segment

    // Draw the gallows
    if (wrongGuesses > 0)
    {
        // Vertical pole
        MoveToEx(hdc, baseX, baseY, NULL);
        LineTo(hdc, baseX, baseY - 200);

        // Horizontal pole
        MoveToEx(hdc, baseX - 20, baseY - 200, NULL);
        LineTo(hdc, baseX + 70, baseY - 200);

        // Rope
        MoveToEx(hdc, baseX + 50, baseY - 200, NULL);
        LineTo(hdc, baseX + 50, baseY - 180); // Adjusted to connect to head
    }

    // Draw the head
    if (wrongGuesses > 1)
    {
        // Center of the head
        int headCenterX = baseX + 50;
        int headCenterY = baseY - 170;

        // Draw head
        Ellipse(hdc, headCenterX - headRadius, headCenterY - headRadius,
            headCenterX + headRadius, headCenterY + headRadius);
    }

    // Draw the body
    if (wrongGuesses > 2)
    {
        MoveToEx(hdc, baseX + 50, baseY - 150, NULL);
        LineTo(hdc, baseX + 50, baseY - 100);
    }

    // Draw the left arm
    if (wrongGuesses > 3)
    {
        MoveToEx(hdc, baseX + 50, baseY - 150, NULL);
        LineTo(hdc, baseX + 30, baseY - 130);
    }

    // Draw the right arm
    if (wrongGuesses > 4)
    {
        MoveToEx(hdc, baseX + 50, baseY - 150, NULL);
        LineTo(hdc, baseX + 70, baseY - 130);
    }

    // Draw the left leg
    if (wrongGuesses > 5)
    {
        MoveToEx(hdc, baseX + 50, baseY - 100, NULL);
        LineTo(hdc, baseX + 30, baseY - 80);
    }

    // Draw the right leg
    if (wrongGuesses > 6)
    {
        MoveToEx(hdc, baseX + 50, baseY - 100, NULL);
        LineTo(hdc, baseX + 70, baseY - 80);
    }
}