#include <Windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <dwmapi.h>
#include <TlHelp32.h>
#include <string>
#include "memory.h"

/*
how02
Recruit
s0beit
Grab
DarkLinuxz
http://www.unknowncheats.me/
*/

Memory memory;

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

#include "hDirectX.h"

int Width = 800;
int Height = 600;

HWND hWnd;
MSG Message;
HWND TargetWnd;
RECT TargetRect;
RECT CheckWnd;

const MARGINS Margin = 
{ 
	0, 
	0, 
	Width,
	Height
};

LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_PAINT:
		RenderIt();
		break;

	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &Margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(1);
		break;

	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}

	return 0;
}
HWND TargetWindow;
RECT TargetSize;
void SetWindowToTarget();
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR CmdLine, int CmdShow)
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEX OverlayWnd;

	OverlayWnd.cbClsExtra		= 0;
	OverlayWnd.cbSize			= sizeof(WNDCLASSEX);
	OverlayWnd.cbWndExtra		= 0;
	OverlayWnd.hbrBackground	= (HBRUSH)CreateSolidBrush(RGB(0,0,0));
	OverlayWnd.hCursor			= LoadCursor(0, IDC_ARROW);
	OverlayWnd.hIcon			= LoadIcon(0, IDI_APPLICATION);
	OverlayWnd.hIconSm			= LoadIcon(0, IDI_APPLICATION);
	OverlayWnd.hInstance		= hInstance;
	OverlayWnd.lpfnWndProc		= Proc;
	OverlayWnd.lpszClassName	= "Overlay";
	OverlayWnd.lpszMenuName		= "Overlay";
	OverlayWnd.style			= CS_HREDRAW | CS_VREDRAW;

	if(!RegisterClassEx(&OverlayWnd))
		exit(1);

	//hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_COMPOSITED | WS_EX_TRANSPARENT,  "Overlay", "Overlay",WS_EX_TOPMOST | WS_POPUP  , 100, 100, 1024, 786, NULL, NULL, NULL, NULL);
	//SetLayeredWindowAttributes(hWnd, RGB(0,0,0), 0, ULW_COLORKEY);
	//SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
	//ShowWindow(hWnd, SW_SHOW);



	TargetWindow = FindWindow(0, "Battlefield 4");

	if (TargetWindow)
	{
		GetWindowRect(TargetWindow, &TargetSize);

		Width = TargetSize.right - TargetSize.left;
		Height = TargetSize.bottom - TargetSize.top;

		hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, "Overlay", "Overlay", WS_POPUP, 1, 1, Width, Height, 0, 0, 0, 0);
		SetLayeredWindowAttributes(hWnd, 0, 1.0f, LWA_ALPHA);
		SetLayeredWindowAttributes(hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);

		ShowWindow(hWnd, SW_SHOW);

		SetForegroundWindow(TargetWindow);
	}



	D3DIniti(hWnd);

	memory.init("bf4.exe");



	for (;;)
	{
		if (PeekMessage(&Message, hWnd, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&Message);
			TranslateMessage(&Message);
		}

		//Sleep(1);
	}

	/*for(;;)
	{
		if(PeekMessage(&Message, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		TargetWnd = FindWindow(0, "Battlefield 4");
		GetWindowRect( TargetWnd, &TargetRect);
		MoveWindow( hWnd, TargetRect.left, TargetRect.top, TargetRect.right - TargetRect.left, TargetRect.bottom - TargetRect.top, true);
		
		if(!TargetWnd)
		{
			exit(0);
		}

		RenderIt();
	}*/
	return 0;
}


void SetWindowToTarget()
{
	// Diese Methode soll mir das Spiel Fenster in den Vordergrund bringen und die
	// aktuelle Größe und Position auslesen.

	while (true)
	{
		TargetWindow = FindWindow(0, "Battlefield 4");

		if (TargetWindow)
		{
	
			GetWindowRect(TargetWindow, &TargetSize);

			Width = TargetSize.right - TargetSize.left;
			Height = TargetSize.bottom - TargetSize.top;

			Width -= 1;

			DWORD Style = GetWindowLong(TargetWindow, GWL_STYLE);

			if (Style & WS_BORDER)
			{
				TargetSize.top += 21;
				Height -= 21;
			}

			MoveWindow(hWnd, TargetSize.left, TargetSize.top, Width, Height, true);
		}
		else
		{
			// Beenden mit negativer Flag
			exit(1);
		}

		// Damit es nicht zu schnell läuft, einen 100 Millisekunden stop
		Sleep(100);
	}
}
