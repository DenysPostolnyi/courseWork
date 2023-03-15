﻿#include <windows.h> // підключення бібліотеки з функціями API
#include "Resource.h"

// Глобальні змінні:
HINSTANCE hInst;   //Дескриптор програми  
LPCTSTR szWindowClass = TEXT("QWERTY");
LPCTSTR szTitle = TEXT("МЛИН");
LPCTSTR szListText;
HRESULT lResult;


// Попередній опис функцій

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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
    wcex.lpszMenuName = NULL;         //визначення меню
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

// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// Віконна процедура. Приймає і обробляє всі повідомлення, що приходять в додаток

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE: //Повідомлення приходить при створенні вікна
        break;

    case WM_COMMAND:
        break;

    case WM_DESTROY:         //Завершення роботи
        PostQuitMessage(0);
        break;
    default:
        //Обробка повідомлень, які не оброблені користувачем
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
