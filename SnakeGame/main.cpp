// 例子模拟系统.cpp : 定义应用程序的入口点。
//
#include "resource.h"
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "Snake.h"
#pragma comment(lib, "user32")
#pragma comment(lib, "kernel32")
#pragma comment(lib, "gdi32")

#define WIDTH    697 
#define HIEIGHT  600 

#define OUTSKIRTWIDTH 40
#define OUTSKIRTHEIGH 40

LPCTSTR szTitle = TEXT("贪食蛇游戏 —— by 喵喵");	// 标题栏文本
LPCTSTR szWindowClass = TEXT("Snake");			    // 主窗口类名
TCHAR buf[100];

float SceneWidth, SceneHeight;  // 屏幕尺寸 
double DeltaTime;  // 两次显示的间隔时间,计算FPS用 
int nMoveTime;  // 多长时间移动一次，毫秒为单位 
int nScore;  // 当前分值 
BOOL nButtonFlags = FALSE;
HBITMAP hImageBitmap;


// 不断的计算时间，画图消息  
DWORD WINAPI Moni(LPVOID lparam)
{
    HWND hWnd = (HWND)lparam;
    LARGE_INTEGER TimeFreq, LastTime, ThisTime;
    DWORD Timetms, NextTimetms;
    SnakeElement element;
    int nCount = 0; // 计数器，获取个数多少个将加速 

    QueryPerformanceFrequency(&TimeFreq);
    QueryPerformanceCounter(&ThisTime);
    LastTime = ThisTime;
    Timetms = GetTickCount();

    nScore = 0;
    nMoveTime = 1000; // 初始速度1s一次 
    SetOutskirtLimit(OUTSKIRTWIDTH, OUTSKIRTHEIGH);
    InitGame();
    do {
        LastTime = ThisTime;

        QueryPerformanceCounter(&ThisTime);
        DeltaTime = ((double)(ThisTime.QuadPart - LastTime.QuadPart))/TimeFreq.QuadPart;

        InvalidateRect(hWnd, NULL, FALSE);
        SendMessage(hWnd, WM_PAINT, 0, 0);
        NextTimetms = GetTickCount();
        if (nButtonFlags)
        {
            // 立马移动一个位置 
            Timetms = 0;
            nButtonFlags = FALSE;
        }
        if (NextTimetms - Timetms >= nMoveTime)
        {
            Timetms = NextTimetms;
            if( MoveSnake(&element) != GAME_NORMAL)
            {
                MessageBox(hWnd, TEXT("游戏结束,将重新开始游戏"), TEXT("提示"), MB_OK);

                // 重新开始
                InitGame();
                nMoveTime = 1000;
                nScore = 0;
                nCount = 0;
            }
            if ((element&SNAKE_FOOD_MASK) == SNAKE_FOOD_MASK)
            {
                nCount++;
                if (nCount%5 == 0)
                {
                    switch(nCount/5)
                    {
                    case 1:
                    case 2:
                        nMoveTime -= 200;
                        break;
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                        nMoveTime -= 100;
                        break;
                    default:
                        nMoveTime -= 30;
                    }
                }

                switch (element^SNAKE_FOOD_MASK)
                {
                case 0x00000001:
                    nScore += 2;
                    break;
                case 0x00000002:
                    nScore += 5;
                    break;
                }
            }
        }
    } while (1); 
    return 0;
}

// 绘图函数 
LRESULT CALLBACK MyPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT Rect;
    PAINTSTRUCT ps;

    hdc = BeginPaint(hWnd, &ps); // 获取需要绘画DC，推荐此方法，绘图效率高
    GetClientRect(hWnd, &Rect);

    // 创建内存DC，先画内存DC，加速画画速度，避免产生闪耀
    HDC hMemoryDC = CreateCompatibleDC(hdc);
    HBITMAP hMemoryMap = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom);
    SelectObject(hMemoryDC, hMemoryMap);

    // 刷白色背景 
    HBRUSH hr = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hMemoryDC, hr);
    FillRect(hMemoryDC, &Rect, hr);

    // 显示帧数 
    if (DeltaTime > 0.0)
    {
        _stprintf(buf, TEXT("FPS:%f"), 1.0/DeltaTime);  // 显示正确的FPS 
        TextOut(hMemoryDC, 5, 5, buf, _tcslen(buf));
        if (DeltaTime > 0.1)
        {
            DeltaTime = 0.1;
        }

        _stprintf(buf, TEXT("Score: %u"), nScore);  // 显示当前分值  
        TextOut(hMemoryDC, 5, 25, buf, _tcslen(buf));
    }

    int WidthPer = SceneWidth/OUTSKIRTWIDTH;
    int HeightPer = SceneHeight/OUTSKIRTHEIGH;

    // 画外框 
    HPEN hPen = CreatePen(PS_SOLID,0,RGB(0,0,0));  // 黑色笔  
    MoveToEx(hMemoryDC, 0, 0, NULL);
    LineTo(hMemoryDC, 0, HeightPer*OUTSKIRTHEIGH);
    LineTo(hMemoryDC, WidthPer*OUTSKIRTWIDTH, HeightPer*OUTSKIRTHEIGH);
    LineTo(hMemoryDC, WidthPer*OUTSKIRTWIDTH, 0);
    LineTo(hMemoryDC, 0, 0);
    DeleteObject(hPen);

    // 循环画圆 
    SnakeElement* snake = GetSnakeState();
    if (snake)
    {
        for (int x=0; x<OUTSKIRTWIDTH; x++)
        {
            for (int y=0; y<OUTSKIRTHEIGH; y++)
            {
                int tmp = snake[y*OUTSKIRTWIDTH + x];
                if ( (tmp&SNAKE_BODY_MASK) == SNAKE_BODY_MASK )
                {
                    // 蛇 
                    HPEN hPen = CreatePen(PS_SOLID,0,RGB(0,0,0));  // 图形边界颜色，黑色 
                    HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));  // 物体颜色 
                    SelectObject(hMemoryDC, hPen);  
                    SelectObject(hMemoryDC, hBrush); 
                    // 单独话蛇头好像有点丑，哈哈哈哈 
                    if ( (tmp^SNAKE_BODY_MASK) == 0)
                    {
                        RoundRect(hMemoryDC, WidthPer*x + 2, HeightPer*y + 2, WidthPer*(x+1) - 2, HeightPer*(y+1) - 2, WidthPer/3, HeightPer/3); 
                    }
                    else
                    {
                        RoundRect(hMemoryDC, WidthPer*x + 2, HeightPer*y + 2, WidthPer*(x+1) - 2, HeightPer*(y+1) - 2, 0, 0); 
                    }
                    DeleteObject(hPen);
                    DeleteObject(hBrush);
                }
                else if ((tmp&SNAKE_FOOD_MASK) == SNAKE_FOOD_MASK)
                {
                    // 食物 
                    HPEN hPen = CreatePen(PS_SOLID,0,RGB(255,0,0));  // 图形边界颜色，黑色 
                    HBRUSH hBrush = CreateSolidBrush(RGB(255,0,0));  // 物体颜色 
                    SelectObject(hMemoryDC, hPen);  
                    SelectObject(hMemoryDC, hBrush); 
                    if (tmp == SNAKE_FOOD)
                    {
                        // 普通食物是圆形的 
                        int R = WidthPer>HeightPer?HeightPer:WidthPer; 
                        Ellipse(hMemoryDC, WidthPer*x + 2, HeightPer*y + 2, WidthPer*(x+1) - 2, HeightPer*(y+1) - 2);
                    }
                    else if (tmp == SNAKE_REWARD)
                    {
                        // 奖励小红花 
                        BITMAP bitmap;
                        HDC hBitmapDc = CreateCompatibleDC(hdc);
                        SelectObject(hBitmapDc, hImageBitmap); 
                        GetObject(hImageBitmap, sizeof(bitmap),&bitmap);
                        StretchBlt(hMemoryDC, 
                            WidthPer*x + 2, HeightPer*y + 2, WidthPer - 4, HeightPer - 4, 
                            hBitmapDc, 
                            0, 0, bitmap.bmWidth,bitmap.bmHeight, 
                            SRCCOPY);
                        DeleteDC(hBitmapDc);
                    }
                    DeleteObject(hPen);
                    DeleteObject(hBrush);
                }
            }

        }
    }

    // 复制到屏幕显示
    BitBlt(hdc, 0, 0, Rect.right, Rect.bottom, hMemoryDC, 0, 0, SRCCOPY); // 将内存DC，输出到控件 
    EndPaint(hWnd, &ps); // 完成绘图
    DeleteObject(hr);
    DeleteObject(hMemoryMap);
    DeleteDC(hMemoryDC);
    return 0;
}

// message loop 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            RECT Rect;
            GetClientRect(hWnd, &Rect);
            SceneWidth = Rect.right;
            SceneHeight = Rect.bottom;
            SetTimer(hWnd, 0, 500, NULL);
        }
        break;

    case WM_TIMER:
        KillTimer(hWnd, wParam);
        CloseHandle(CreateThread(NULL, 0, Moni, hWnd, 0, NULL));
        break;

    case WM_KEYDOWN:
        if (wParam>0x20 && wParam <=0x7E)
        {
            switch(wParam)
            {
            case VK_UP:
                SetMoveDirection(SNAKE_UP);
                nButtonFlags = TRUE;
                break;
            case VK_DOWN:
                SetMoveDirection(SNAKE_DOWN);
                nButtonFlags = TRUE;
                break;
            case VK_LEFT:
                SetMoveDirection(SNAKE_LEFT);
                nButtonFlags = TRUE;
                break;
            case VK_RIGHT:
                SetMoveDirection(SNAKE_RIGHT);
                nButtonFlags = TRUE;
                break;
            default:
                break;
            }
        }
        break;

    case WM_SIZE:
        {
            RECT Rect;
            GetClientRect(hWnd, &Rect);
            SceneWidth = Rect.right;
            SceneHeight = Rect.bottom;
            return 1;
        }
        break;

    case WM_PAINT:
        MyPaint(hWnd, message, wParam, lParam);
        break;

    case WM_DESTROY:
        ReleaseGame();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// register window class 
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    HICON hIcon;

    hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1001));

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= hIcon;
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= NULL;
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= hIcon;

    return RegisterClassEx(&wcex);
}

// Create and show Window
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    int nWidth = (GetSystemMetrics(SM_CXSCREEN) - WIDTH)>>1;  
    int nHeight = (GetSystemMetrics(SM_CYSCREEN) - HIEIGHT)>>1;  
    hWnd = CreateWindowEx(/*WS_EX_TOOLWINDOW|WS_EX_TOPMOST*/ NULL, 
        szWindowClass, 
        szTitle, 
        WS_OVERLAPPEDWINDOW,
        nWidth, 
        nHeight, 
        WIDTH, 
        HIEIGHT, 
        NULL,  // Parent Window 
        NULL,  // Menu 
        hInstance, 
        NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MSG msg;

    srand(time(NULL));    

//     hImageBitmap = (HBITMAP)LoadImage(hInstance, TEXT("D:\\flower.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hImageBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDC_FLOWER));
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // 主消息循环:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(hImageBitmap);
    return (int) msg.wParam;
}