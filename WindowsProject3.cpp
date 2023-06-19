#include <windows.h>
#include <stdio.h>
#include "TickCounter.h"
#include "Key.h"
#include "control.h"
//全局变量声明
HDC		hdc, mdc;
HWND	hWnd;
DWORD	tPre, tNow;
Controller control;

//全局函数声明
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);

TickCounter tick_counter;
// get time x ms
long Time() {
	return tick_counter.Now();
}

//****WinMain函数，程序入口点函数***********************
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow) {
	MSG msg;

	MyRegisterClass(hInstance);

	//初始化
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	GetMessage(&msg, NULL, NULL, NULL);//初始化msg              
	//游戏循环
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				break;
			}
		}
		control.updateTime();
		control.check();
		MyPaint(hdc);
	}
	return msg.wParam;
}

//****设计一个窗口类****
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "canvas";
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

//****初始化函数****
// 加载位图并设定各种初始值
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hWnd = CreateWindow("canvas", "musicGame", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		return FALSE;
	}

	MoveWindow(hWnd, 10, 10, 900, 900, true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	////////////////////////////////////////
	HBITMAP fullmap;
	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);

	//建立空的位图并置入mdc中
	fullmap = CreateCompatibleBitmap(hdc, 900, 900);
	SelectObject(mdc, fullmap);

	MyPaint(hdc);

	return TRUE;
}

//****自定义绘图函数****
// 人物贴图坐标修正及窗口贴图

void MyPaint(HDC hdc) {
	control.paint(mdc);
	BitBlt(hdc, 0, 0, 900, 900, mdc, 0, 0, SRCCOPY);
	//tPre = Time();         //记录此次绘图时间
}

//****消息处理函数***********************************
// 1.按下【Esc】键结束程序

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYDOWN:	     //按下键盘消息
		//判断按键的虚拟键码
		switch (wParam) {
		case VK_ESCAPE:           //按下【Esc】键
			control.passKey('p', 1);
			break;
		case VK_I:
		case VK_O:
		case VK_D:				//按下 D 键
		case VK_C:
		case VK_F:				//按下 F 键
		case VK_J:				//按下 J 键
		case VK_K:				//按下 K 键
		case VK_S:
		case VK_P:
		case VK_Q:
		case VK_R:		
			control.passKey(wParam - 0x41 + 'a', 1);
			break;
		}
		break;
	case WM_KEYUP:	     //抬起键盘消息
		//判断按键的虚拟键码
		switch (wParam) {
		case VK_D:				//松开 D 键
		case VK_F:				//松开 F 键
		case VK_J:				//松开 J 键
		case VK_K:				//松开 K 键
			control.passKey(wParam - 0x41 + 'a', 0);
			break;
		}
		break;
	case WM_DESTROY:			    	//窗口结束消息
		DeleteDC(mdc);

		ReleaseDC(hWnd, hdc);
		
		PostQuitMessage(0);
		break;
	default:							//其他消息
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

