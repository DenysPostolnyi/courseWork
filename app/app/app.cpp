#include <windows.h> // підключення бібліотеки з функціями API
#include "Resource.h"
#include <tchar.h>
#include <fstream>
#include <string>


// Глобальні змінні:
HINSTANCE hInst;   //Дескриптор програми  
LPCTSTR szWindowClass = TEXT("QWERTY");
LPCTSTR szTitle = TEXT("МЛИН");
LPCTSTR szListText;
HRESULT lResult;

struct CONTROLS // для дочерних окон
{
    HWND hControl;
    int id; // индитивикатор окна
    int state; // состояние кнопок
};

CONTROLS myControls[6];

// Попередній опис функцій

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool putElement(HWND hWnd, int x, int y, HDC hdcPaint);
bool checkThree(int place);
bool removeEnemysItem(int x, int y, wchar_t colorToRemove, HDC hdcPaint);
LPCWSTR findWinner();
void resetGame(HDC hdcPaint);

// Основна програма 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
    int nCmdShow)
{
    MSG msg;

    // Реєстрація класу вікна 
    MyRegisterClass(hInstance);

    // Створення вікна програми
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    // Цикл обробки повідомлень
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;     //стиль вікна
    wcex.lpfnWndProc = (WNDPROC)WndProc;     //віконна процедура
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;       //дескриптор програми
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));     //визначення іконки
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);   //визначення курсору
    wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW); //установка фону
    wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); //визначення меню
    wcex.lpszClassName = szWindowClass;     //ім’я класу
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);       //реєстрація класу вікна
}

// FUNCTION: InitInstance (HANDLE, int)
// Створює вікно програми і зберігає дескриптор програми в змінній hInst

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance; //зберігає дескриптор додатка в змінній hInst
    hWnd = CreateWindow(szWindowClass,   // ім’я класу вікна
        szTitle,         // назва програми
        WS_OVERLAPPEDWINDOW,      // стиль вікна
        CW_USEDEFAULT,       // положення по Х  
        CW_USEDEFAULT,      // положення по Y 
        1024,       // розмір по Х
        768,       // розмір по Y
        NULL,           // дескриптор батьківського вікна  
        NULL,           // дескриптор меню вікна
        hInstance,         // дескриптор програми
        NULL);         // параметри створення.
    if (!hWnd)   //Якщо вікно не творилось, функція повертає FALSE
    {
        return FALSE;
    }
    ShowWindow(hWnd, nCmdShow);     //Показати вікно
    UpdateWindow(hWnd);         //Оновити вікно
    return TRUE;
}

TCHAR player1Black[100];
TCHAR player2Red[100];

INT_PTR CALLBACK DlgLogin(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hEdit;
    HWND hEdit2;
    switch (message)
    {
    case WM_INITDIALOG:  //ініціалізація функціоналу керування діалоговим вікном
    {
        // Create an Edit control
        hEdit = CreateWindowEx(
            0,
            L"edit",
            L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
            50, 65, 200, 20,
            hwnd, (HMENU)myControls[4].id,
            hInst, NULL);
        
        hEdit2 = CreateWindowEx(
            0,
            L"edit",
            L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
            50, 130, 200, 20,
            hwnd, (HMENU)myControls[5].id,
            hInst, NULL);

        return TRUE;
    }

        //цикл обробки натискання елементів на формі діалогового вікна
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == EN_SETFOCUS)
        {
            // The edit control has received focus, return TRUE to indicate that we have handled the message
            return TRUE;
        }
        if (HIWORD(wParam) == EN_KILLFOCUS)
        {
            // The edit control has received focus, return TRUE to indicate that we have handled the message
            return TRUE;
        }
        switch (LOWORD(wParam))
        {
        case 4:
            return TRUE;
        case 5:
            return TRUE;


        case IDOK:
        {

            // Retrieve text from the Edit control
            GetDlgItemText(hwnd, myControls[4].id, player1Black, sizeof(player1Black) - 1);
            GetDlgItemText(hwnd, myControls[5].id, player2Red, sizeof(player2Red) - 1);

            if (_tcslen(player1Black) > 0 && _tcslen(player2Red) > 0) {
                EndDialog(hwnd, 0);//знищення модального діалогового вікна
                return FALSE;
            }
            MessageBox(hwnd, TEXT("Заповніть усі поля"), TEXT("Помилка"), MB_OK);
            return TRUE;

        }
        }
    }
        
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return TRUE;
    }
       
    }
    return FALSE;
}

INT_PTR CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        std::ifstream file("../results.txt");
        if (file.is_open())
        {
            std::string line;
            HWND hListBox = GetDlgItem(hwnd, IDC_LIST1);

            while (std::getline(file, line))
            {
                SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)line.c_str());
            }

            file.close();
        }
        else
        {
            MessageBox(hwnd, L"Error opening the file.", L"Error", MB_OK | MB_ICONERROR);
            EndDialog(hwnd, 0);
        }

        return TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hwnd, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK DlgMenu(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::wstring file_contents;
    switch (message)
    {
    case WM_INITDIALOG:  //ініціалізація функціоналу керування діалоговим вікном
        return TRUE;

            //цикл обробки натискання елементів на формі діалогового вікна
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
            {
            case ID_CLOSEGAME:
            {
                PostQuitMessage(0);//знищення модального діалогового вікна
                return TRUE;
            }
            case IDC_GAME:
            {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hwnd, DlgLogin);
                EndDialog(hwnd, 0);//знищення модального діалогового вікна
                return FALSE;
            }
            case IDC_ABOUT:
            {
                MessageBox(hwnd, TEXT("В якості ігрових фігур використовуються 9 чорних, 9 красних круглих фішок та спеціальне ігрове поле.\nГравці по черзі виставляють фішки на поле, намагаючись зібрати ряд з трьох фішок одного кольору.Як тільки комусь це вдалося, він забирає собі одну з фішок суперника.Таким чином, потрібно захопити максимум фішок опонента, не залишивши йому можливості збирати ряди.\nПеремагає той гравець, у якого наприкінці гри залишається більше фігур на полі."), TEXT("Правила гри"), NULL);
                return FALSE;
            }
            case IDC_OWNER:
            {
                MessageBox(hwnd, TEXT("Студент групи КІУКІ-21-10, Постольний Денис Олексійович"), TEXT("Створив гру"), NULL);
                return FALSE;
            }
            case IDC_RESULTS:
            {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hwnd, DialogProc);
                
                return FALSE;

            }
            }
    }
            
    case WM_CLOSE:
        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}

// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// Віконна процедура. Приймає і обробляє всі повідомлення, що приходять в додаток
struct PLACE {
    wchar_t colorEl = NULL;
    RECT coord;
};

const int fieldSize = 24;

PLACE field[fieldSize];

int x1 = 270, yOne = 100, x2 = 820, y2 = 650; // коорд большого квадрата
int dif = 80; // разница между квадратами

RECT rtBig = { x1, yOne, x2, y2 };
RECT rtMiddle = { x1 + dif, yOne + dif, x2 - dif, y2 - dif };
RECT rtSmall = { x1 + dif * 2, yOne + dif * 2, x2 - dif * 2, y2 - dif * 2 };


LPCWSTR BlackWindowName;
WCHAR BlackWindowTitle[250];

LPCWSTR RedWindowName;
WCHAR RedWindowTitle[250];

LPCWSTR turn = TEXT("Turn: ");
WCHAR turnTitle[20];

static short black = 9;
static short red = 9;
bool whoseTurn = 0; // 0 - black, 1 - red
bool checkRemove = false;

HBRUSH hBrush = CreateSolidBrush(RGB(220, 240, 94));

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static TCHAR buff1[100];
    static TCHAR buff2[100];
   
    static int x = 0;
    static int y = 0;
    static int fieldCount = 0;
    static int difer = 0;
    static HBITMAP hBitmap;
    RECT rcClient;
    HDC hdcMem;
    HBITMAP hBitmapOld;
    BITMAP bmp = {};

    int xSrc;
    int ySrc;
    int xDest;
    int yDest;
    int width;
    int height;

    switch (message)
    {
    case WM_CREATE: //Повідомлення приходить при створенні вікна
        for (int i = 0; i < 6; ++i) { // даем ид каждому окну
            myControls[i].id = i;
        }

        // first
        for (int i = 0; i < 3; i++) {
            field[fieldCount].coord = { (x1 + difer) - 15, (yOne + difer) - 15, (x1 + difer) + 15, (yOne + difer) + 15 };
            fieldCount++;
            field[fieldCount].coord = { (x2 - difer + x1 + difer) / 2 - 15, (yOne + difer) - 15, (x2 - difer + x1 + difer) / 2 + 15, (yOne + difer) + 15 };
            fieldCount++;
            field[fieldCount].coord = { (x2 - difer) - 15, (yOne + difer) - 15, (x2 - difer) + 15, (yOne + difer) + 15 };
            fieldCount++;

            difer += 80;
        }

        difer = 0;

        // middle
        for (int i = 0; i < 6; i++) {
            field[fieldCount].coord = { (x1 + difer) - 15, (y2 + yOne) / 2 - 15, (x1 + difer) + 15, (y2 + yOne) / 2 + 15 };
            fieldCount++;
            if (i == 2) {
                difer += 230;
            }
            else {
                difer += 80;
            }
        }

        difer = 160;

        // last
        for (int i = 0; i < 3; i++) {
            field[fieldCount].coord = { (x1 + difer) - 15, (y2 - difer) - 15, (x1 + difer) + 15, (y2 - difer) + 15 };
            fieldCount++;
            field[fieldCount].coord = { (x2 - difer + x1 + difer) / 2 - 15, (y2 - difer) - 15, (x2 - difer + x1 + difer) / 2 + 15, (y2 - difer) + 15 };
            fieldCount++;
            field[fieldCount].coord = { (x2 - difer) - 15, (y2 - difer) - 15, (x2 - difer) + 15, (y2 - difer) + 15 };
            fieldCount++;

            difer -= 80;
        }
        DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgMenu);

        lstrcpy(buff1, player1Black);
        BlackWindowName = lstrcat(buff1, TEXT(": "));
        wsprintf(BlackWindowTitle, TEXT("%s%d"), BlackWindowName, black);

        lstrcpy(buff2, player2Red);
        RedWindowName = lstrcat(buff2, TEXT(": "));
        wsprintf(RedWindowTitle, TEXT("%s%d"), RedWindowName, red);

        myControls[1].hControl = CreateWindow(
            TEXT("static"),
            BlackWindowTitle,
            WS_CHILD | WS_VISIBLE,
            10, 10, 200, 15,
            hWnd, (HMENU)myControls[1].id,
            hInst, NULL
        );

        myControls[2].hControl = CreateWindow(
            TEXT("static"),
            RedWindowTitle,
            WS_CHILD | WS_VISIBLE,
            10, 30, 200, 15,
            hWnd, (HMENU)myControls[2].id,
            hInst, NULL
        );

        myControls[3].hControl = CreateWindow(
            TEXT("static"),
            L"",
            WS_CHILD | WS_VISIBLE,
            10, 50, 80, 15,
            hWnd, (HMENU)myControls[3].id,
            hInst, NULL
        );
        wsprintf(turnTitle, TEXT("%s%s"), turn, player1Black);
        lResult = SendMessageW(myControls[3].hControl, (UINT)WM_SETTEXT, 0, (LPARAM)turnTitle);

        hBitmap = (HBITMAP)LoadImage(nullptr, L"bitmap1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap == nullptr)
        {
            MessageBox(hWnd, L"Failed to load image", L"Error", MB_ICONERROR);
            return -1;
        }
        break;

    case WM_LBUTTONDOWN: // устанавливает где нажата мышь
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        hdc = GetDC(hWnd);

        if (checkRemove == true) {
            if (removeEnemysItem(x, y, (whoseTurn == 0) ? 'b' : 'r', hdc)) {
                checkRemove = false;
                if (red != 0 || black != 0) {
                    MessageBox(hWnd, TEXT("Continue game"), TEXT("Message"), NULL);
                    wsprintf(turnTitle, TEXT("%s%s"), turn, (whoseTurn == 1) ? player2Red : player1Black);
                    lResult = SendMessageW(myControls[3].hControl, (UINT)WM_SETTEXT, 0, (LPARAM)turnTitle);
                }
            }
        }
        else {
            if (whoseTurn == 0) {
                HBRUSH hbrushBlack = CreateSolidBrush(RGB(0, 0, 0)); // black
                SelectObject(hdc, hbrushBlack);
                if (putElement(hWnd, x, y, hdc)) {
                    MessageBox(hWnd, TEXT("Choose red chip to remove"), TEXT("Message to black"), NULL);
                    checkRemove = true;
                }

                DeleteObject(hbrushBlack);
            }
            else if (whoseTurn == 1) {
                HBRUSH hbrushRed = CreateSolidBrush(RGB(255, 0, 0)); // Светло-красный
                SelectObject(hdc, hbrushRed);
                if (putElement(hWnd, x, y, hdc)) {
                    MessageBox(hWnd, TEXT("Choose black chip to remove"), TEXT("Message to red"), NULL);
                    checkRemove = true;
                }

                DeleteObject(hbrushRed);

            } 
        }

        if (red == 0 && black == 0 && checkRemove == false) {
            LPCWSTR message = findWinner();
            MessageBox(hWnd, message, TEXT("Finish"), NULL);
            hdc = GetDC(hWnd);
            resetGame(hdc);
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgMenu);

            lstrcpy(buff1, player1Black);
            BlackWindowName = lstrcat(buff1, TEXT(": "));
            wsprintf(BlackWindowTitle, TEXT("%s%d"), BlackWindowName, black);
            lResult = SendMessageW(myControls[1].hControl, (UINT)WM_SETTEXT, 0, (LPARAM)BlackWindowTitle);

            lstrcpy(buff2, player2Red);
            RedWindowName = lstrcat(buff2, TEXT(": "));
            wsprintf(RedWindowTitle, TEXT("%s%d"), RedWindowName, red);
            lResult = SendMessageW(myControls[2].hControl, (UINT)WM_SETTEXT, 0, (LPARAM)RedWindowTitle);

            wsprintf(turnTitle, TEXT("%s%s"), turn, (whoseTurn == 1) ? player2Red : player1Black);
            lResult = SendMessageW(myControls[3].hControl, (UINT)WM_SETTEXT, 0, (LPARAM)turnTitle);

            for (int i = 0; i < 4; ++i) { // даем ид каждому окну
                ShowWindow(myControls[i].hControl, SW_SHOW);
            }

            break;
        }

        ReleaseDC(hWnd, hdc);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            case IDABOUT_GAME:
                MessageBox(hWnd, TEXT("В якості ігрових фігур використовуються 9 чорних, 9 красних круглих фішок та спеціальне ігрове поле.\nГравці по черзі виставляють фішки на поле, намагаючись зібрати ряд з трьох фішок одного кольору.Як тільки комусь це вдалося, він забирає собі одну з фішок суперника.Таким чином, потрібно захопити максимум фішок опонента, не залишивши йому можливості збирати ряди.\nПеремагає той гравець, у якого наприкінці гри залишається більше фігур на полі."), TEXT("Правила гри"), NULL);
                break;
            case ID_OWNER:
                MessageBox(hWnd, TEXT("Студент групи КІУКІ-21-10, Постольний Денис Олексійович"), TEXT("Створив гру"), NULL);
                break;
        }
        break;

    case WM_PAINT:  // Перемалювати вікно
        hdc = BeginPaint(hWnd, &ps);

        RECT rcClient;
        GetClientRect(hWnd, &rcClient);

        hdcMem = CreateCompatibleDC(hdc);
        hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

        GetObject(hBitmap, sizeof(BITMAP), &bmp);

        xSrc = 0;
        ySrc = 0;
        xDest = 0;
        yDest = 0;
        width = rcClient.right - rcClient.left;
        height = rcClient.bottom - rcClient.top;

        StretchBlt(hdc, xDest, yDest, width, height, hdcMem, xSrc, ySrc, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

        SelectObject(hdcMem, hBitmapOld);
        DeleteDC(hdcMem);

        SelectObject(hdc, hBrush);
        Rectangle(hdc, rtBig.left, rtBig.top, rtBig.right, rtBig.bottom);
        
        Rectangle(hdc, rtMiddle.left, rtMiddle.top, rtMiddle.right, rtMiddle.bottom);
        Rectangle(hdc, rtSmall.left, rtSmall.top, rtSmall.right, rtSmall.bottom);

        MoveToEx(hdc, (x1 + x2) / 2, 100, NULL);
        LineTo(hdc, (x1 + x2) / 2, 260);

        MoveToEx(hdc, (x1 + x2) / 2, 490, NULL);
        LineTo(hdc, (x1 + x2) / 2, 650);

        MoveToEx(hdc, x1, (yOne + y2) / 2, NULL);
        LineTo(hdc, x1 + 160, (yOne + y2) / 2);

        MoveToEx(hdc, x2, (yOne + y2) / 2, NULL);
        LineTo(hdc, x2 - 160, (yOne + y2) / 2);

        for (int i = 0; i < fieldSize; i++) {
            Ellipse(hdc, field[i].coord.left, field[i].coord.top, field[i].coord.right, field[i].coord.bottom);
        }

        EndPaint(hWnd, &ps);
        return 0;

    case WM_DESTROY:         //Завершення роботи
        DeleteObject(hBitmap);
        DeleteObject(hBrush);
        PostQuitMessage(0);
        break;
    default:
        //Обробка повідомлень, які не оброблені користувачем
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool putElement(HWND hWnd, int x, int y, HDC hdcPaint) {
    for (int i = 0; i < fieldSize; i++) {
        if ((x >= field[i].coord.left && x <= field[i].coord.right) && (y <= field[i].coord.bottom && y >= field[i].coord.top)) {
            if (field[i].colorEl == NULL) {
                Ellipse(hdcPaint, field[i].coord.left, field[i].coord.top, field[i].coord.right, field[i].coord.bottom);
                if (whoseTurn == 0) {
                    field[i].colorEl = 'b';
                    black--;
                    wsprintf(BlackWindowTitle, TEXT("%s%d"), BlackWindowName, black);
                    lResult = SendMessageW(myControls[1].hControl, (UINT)WM_SETTEXT, 0, (LPARAM)BlackWindowTitle);
                    whoseTurn = 1;
                    if (checkThree(i)) {
                        return true;
                    }
                    wsprintf(turnTitle, TEXT("%s%s"), turn, player2Red);
                    lResult = SendMessageW(myControls[3].hControl, (UINT)WM_SETTEXT, 0, (LPARAM)turnTitle);
                    return false;
                }
                else if (whoseTurn == 1) {
                    field[i].colorEl = 'r';
                    red--;
                    wsprintf(RedWindowTitle, TEXT("%s%d"), RedWindowName, red);
                    lResult = SendMessageW(myControls[2].hControl, (UINT)WM_SETTEXT, 0, (LPARAM)RedWindowTitle);
                    whoseTurn = 0;
                    if (checkThree(i)) {
                        return true;
                    }
                    wsprintf(turnTitle, TEXT("%s%s"), turn, player1Black);
                    lResult = SendMessageW(myControls[3].hControl, (UINT)WM_SETTEXT, 0, (LPARAM)turnTitle);
                    return false;
                }
            }
            break;
        }
    }
    return false;
}

bool checkThree(int place)
{
    const int amountOfCorn = 12;
    int listOfCorn[amountOfCorn] = {
        0, 2,
        3, 5,
        6, 8, 
        15, 17,
        18, 20,
        21, 23
    };

    const int amountOfMidle = 12;
    int listOfMidle[amountOfMidle] = {
        1, 4, 7, 9, 10, 11, 12, 13, 14, 16, 19, 22
    };

    for (int i = 0; i < amountOfMidle; i++) {  // проверям относится ли место к средним элементам
        if (place == listOfMidle[i]) {
            if (place < 9 || place > 14) { // если верхняя палка или нижняя
                if (field[place].colorEl == field[place-1].colorEl && field[place].colorEl == field[place + 1].colorEl) { // проверяем соседние по бокам элементы
                    return true;
                }

                if (i == 0 || i == 9) {
                    if (field[listOfMidle[i]].colorEl == field[listOfMidle[i + 1]].colorEl && field[listOfMidle[i]].colorEl == field[listOfMidle[i + 2]].colorEl) { // проверяем соседние по вертикали элементы сверху вниз
                        return true;
                    }
                }

                if (i == 2 || i == 11) {
                    if (field[listOfMidle[i]].colorEl == field[listOfMidle[i - 1]].colorEl && field[listOfMidle[i]].colorEl == field[listOfMidle[i - 2]].colorEl) { // проверяем соседние по вертикали элементы снизу вверх
                        return true;
                    }
                }
                if (i == 1 || i == 10) {
                    if (field[listOfMidle[i]].colorEl == field[listOfMidle[i - 1]].colorEl && field[listOfMidle[i]].colorEl == field[listOfMidle[i + 1]].colorEl) { // проверяем соседние по вертикали элементы если выбранный элемент по середине
                        return true;
                    }
                }
            }
            else { // если боковые палки
                if (i == 3 || i == 6) {
                    if (field[listOfMidle[i]].colorEl == field[listOfMidle[i] + 1].colorEl && field[listOfMidle[i]].colorEl == field[listOfMidle[i] + 2].colorEl) { // проверяем соседние по горизонтали элементы слева на право 
                        return true;
                    }
                }

                if (i == 5 || i == 8) {
                    if (field[listOfMidle[i]].colorEl == field[listOfMidle[i] - 1].colorEl && field[listOfMidle[i]].colorEl == field[listOfMidle[i] - 2].colorEl) { // проверяем соседние по горизонтали элементы справо на лево
                        return true;
                    }
                }
                if (i == 4 || i == 7) {
                    if (field[listOfMidle[i]].colorEl == field[listOfMidle[i] - 1].colorEl && field[listOfMidle[i]].colorEl == field[listOfMidle[i] + 1].colorEl) { // проверяем соседние по горизонтали элементы если выбранный элемент по середине
                        return true;
                    }
                }
            }
        }
    }

    for (int top = 0, middle = 3, low = 10; top <= 4 && middle <= 5 && low >= 6; top += 2, middle += 1, low -= 2) { // проверка по вертикали слевой стороны
        if (place == listOfCorn[top] || place == listOfMidle[middle] || place == listOfCorn[low]) {
            if (field[listOfCorn[top]].colorEl == field[listOfMidle[middle]].colorEl && field[listOfCorn[top]].colorEl == field[listOfCorn[low]].colorEl) { // проверяем соседние элементы вниз
                return true;
            }
            else {
                break;
            }
        }
    }

    for (int top = 1, middle = 8, low = 11; top <= 5 && middle >= 6 && low >= 7; top += 2, middle -= 1, low -= 2) { // проверка по вертикали справой стороны
        if (place == listOfCorn[top] || place == listOfMidle[middle] || place == listOfCorn[low]) {
            if (field[listOfCorn[top]].colorEl == field[listOfMidle[middle]].colorEl && field[listOfCorn[top]].colorEl == field[listOfCorn[low]].colorEl) { // проверяем соседние элементы вниз
                return true;
            }
            else {
                break;
            }
        }
    }

    // если поставили угловой элемент

    for (int i = 0; i < amountOfCorn; i++) {
        if (place == listOfCorn[i]) {
            if (i % 2 == 0) { // если угловой элемент слева, соседние будут справа от него
                if (field[listOfCorn[i]].colorEl == field[listOfCorn[i] + 1].colorEl && field[listOfCorn[i]].colorEl == field[listOfCorn[i] + 2].colorEl) { // проверяем соседние элементы по правую сторону
                    return true;
                }
            }
            else {
                if (field[listOfCorn[i]].colorEl == field[listOfCorn[i] - 1].colorEl && field[listOfCorn[i]].colorEl == field[listOfCorn[i] - 2].colorEl) { // проверяем соседние элементы по левую сторону
                    return true;
                }
            }
        }
    }
    return false;
}

bool removeEnemysItem(int x, int y, wchar_t colorToRemove, HDC hdcPaint)
{
    for (int i = 0; i < fieldSize; i++) {
        if ((x >= field[i].coord.left && x <= field[i].coord.right) && (y <= field[i].coord.bottom && y >= field[i].coord.top)) {
            if (field[i].colorEl == colorToRemove) {
                SelectObject(hdcPaint, hBrush);
                Ellipse(hdcPaint, field[i].coord.left, field[i].coord.top, field[i].coord.right, field[i].coord.bottom);
                
                field[i].colorEl = NULL;
                return true;
            }
            else {
                return false;
            }
        }
    }
    
    return false;
}

LPCWSTR findWinner() {
    int amountOfRed = 0;
    int amountOfBlack = 0;
    for (int i = 0; i < fieldSize; i++) {
        if (field[i].colorEl == 'b') {
            amountOfBlack++;
        } else if (field[i].colorEl == 'r') {
            amountOfRed++;
        } 
    }

    SYSTEMTIME currentTime;
    GetLocalTime(&currentTime);

    // Создание строки с датой и временем
    std::string dateTimeString = std::to_string(currentTime.wYear) + "-" +
        std::to_string(currentTime.wMonth) + "-" +
        std::to_string(currentTime.wDay) + " " +
        std::to_string(currentTime.wHour) + ":" +
        std::to_string(currentTime.wMinute) + ":" +
        std::to_string(currentTime.wSecond);

    bool check_file;
    std::ofstream file;
    file.open("../results.txt", std::ios_base::app);

    if (file.is_open()) {
        check_file = true;
    }

    size_t bufSizeBlack = 2 * wcslen(player1Black) + 1;
    char* bufBlack = new char[bufSizeBlack];
    size_t convertedBlack = 0;
    wcstombs_s(&convertedBlack, bufBlack, bufSizeBlack, player1Black, wcslen(player1Black));

    size_t bufSizeRed = 2 * wcslen(player2Red) + 1;
    char* bufRed = new char[bufSizeRed];
    size_t convertedRed = 0;
    wcstombs_s(&convertedRed, bufRed, bufSizeRed, player2Red, wcslen(player2Red));

    if (amountOfBlack > amountOfRed) {
        if (check_file) {
            file.write(bufBlack, _tcslen(player1Black));
            file << " переміг ";
            file.write(bufRed, _tcslen(player2Red));
            file << " з різницею у " << amountOfBlack - amountOfRed << " фішек | " << dateTimeString << std::endl;
            file.close();
        }

        delete[] bufBlack;
        delete[] bufRed;
        return lstrcat(player1Black, TEXT(" wins"));  
    }
    else if (amountOfBlack < amountOfRed) {
        if (check_file) {
            file.write(bufRed, _tcslen(player2Red));
            file << " переміг ";
            file.write(bufBlack, _tcslen(player1Black));
            file << " з різницею у " << amountOfRed - amountOfBlack << " фішек | " << dateTimeString << std::endl;
            file.close();
        }

        delete[] bufBlack;
        delete[] bufRed;
        return lstrcat(player2Red, TEXT(" wins"));
    }
    else {
        if (check_file) {
            file << "Гра між ";
            file.write(bufBlack, _tcslen(player1Black));
            file << " та ";
            file.write(bufRed, _tcslen(player2Red));
            file << " закінчилась нічиєю | " << dateTimeString << std::endl;
            file.close();
        }

        delete[] bufBlack;
        delete[] bufRed;
        return TEXT("Nobody wins");
    }
}


void resetGame(HDC hdcPaint) {
    black = 9;
    red = 9;
    whoseTurn = 0; 
    checkRemove = false;
    SelectObject(hdcPaint, hBrush);
    for (int i = 0; i < fieldSize; i++) {
        Ellipse(hdcPaint, field[i].coord.left, field[i].coord.top, field[i].coord.right, field[i].coord.bottom);
        field[i].colorEl = NULL;
    }
    for (int i = 0; i < 4; ++i) { 
        ShowWindow(myControls[i].hControl, SW_HIDE);
    }
}