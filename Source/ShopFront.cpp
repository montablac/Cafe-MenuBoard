#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "Draw.h"

/*

Shop Front program

made for Bliss Cafe to display a menu and scrolling images

uses the directX libary to load images and draw them to the screen

created by: Joshua Print

*/

// Standard windows message handler
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wPram, LPARAM lPram)
{
	int iReturn = 0;

	switch (iMessage)
	{
	case WM_KEYDOWN:

		// If the ESC key is pressed, exit the program
		if (wPram == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
		}
		else
		{
		}
		break;

	case WM_KEYUP:
		//key up
		break;

	case WM_CREATE:
		iReturn = 0;
		break;

	case WM_NCCREATE:
		iReturn = 1;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		iReturn = 1;
		break;

	case WM_DISPLAYCHANGE:
		InvalidateRect(hwnd, NULL, false);
		iReturn = 0;
		break;

	case WM_PAINT:
		ValidateRect(hwnd, NULL);
		iReturn = 0;
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		iReturn = 1;
		break;


	default:
		DefWindowProc(hwnd, iMessage, wPram, lPram);
		break;
	}

	return iReturn;
}

// Windows Entry point
int CALLBACK WinMain(HINSTANCE Hinstance, HINSTANCE HinstanceLast, LPSTR cmdLine, int iShow)
{
	DWORD Error = 0;

	MSG msg;

	int iReturn = 0;

	// Setup all the required varibles for Windows
	static TCHAR szWindowClass[] = _TCHAR_DEFINED("Cafe Menu");
	static TCHAR szWindowTitle[] = _TCHAR_DEFINED("Cafe Menu");
	static TCHAR szWindowName[] = _TCHAR_DEFINED("Cafe Menu");

	WNDCLASSEX wcex;
	HWND hwnd = 0;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = Hinstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	// Register the windows class
	int Atom = RegisterClassEx(&wcex);

	Error = GetLastError();

	if (Error == 0 && Atom != 0)
	{
		// Get the Screen size
		// the dimentions dont include the taskbar, for some odd reason
		int iXSize = GetSystemMetrics(SM_CXFULLSCREEN);
		int iYSize = GetSystemMetrics(SM_CYFULLSCREEN) + 63;

		Screen::SetScreenSize(iXSize, iYSize);

		// Create the window
		// it should have no border, and be on top of everything, taking up the whole screen
		hwnd = CreateWindowEx(WS_EX_TOPMOST, szWindowClass, szWindowTitle, (WS_OVERLAPPED | WS_POPUP), 0, 0, iXSize, iYSize, NULL, NULL, Hinstance, NULL);
		Error = GetLastError();

		if (hwnd != 0 && Error == 0)
		{
			// Display the window
			ShowWindow(hwnd, iShow);

			bool bIsMessage;

			Screen View;

			// Startup the system for storing and drawing the immages
			if (View.Startup(hwnd))
			{
				int iQuit = 0;

				bool bError = false;

				do
				{
					// Check for messages from windows
					bIsMessage = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
					if (bIsMessage)
					{
						// Deal with it if we have it...
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					else
					{
						// ...otherwise draw the next frame of the nice animations
						View.Update();
						View.Draw();
					}

				// Exit when windows tells us to quit, or we have an error
				} while ((msg.message != WM_QUIT || iQuit != 0) && !bError);
			}
			else
			{
				
			}
		}
		else
		{
			Error = GetLastError();

			// If we couldent make the window to display
			MessageBoxEx(NULL, "Failed to Show Window.\nError Code:WINDOW2", "Failed to Create Critical Resource", MB_OK | MB_ICONERROR, 0);
			iReturn = 1;
		}
	}
	else
	{
		// If we failed to set the windows class or the ATOM object is bad
		MessageBoxEx(NULL, "Failed to Create Window Object.\nError Code:WINDOW1", "Failed to Create Critical Resource", MB_OK | MB_ICONERROR, 0);
		iReturn = 1;
	}

	return iReturn;
}