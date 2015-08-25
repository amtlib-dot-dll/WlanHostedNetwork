#include "stdafx.h"
#include "MainApplication.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	PVOID pDummy;
	WNDCLASS window_class;
	int nLength;
	LPTSTR lpBuffer;
	MSG msg;
	HWND hWnd;
#pragma region Window Specification
	window_class.lpszClassName = TEXT("WlanHostedNetwork");
	window_class.hInstance = hInstance;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 3 * sizeof(LONG_PTR);
	window_class.style = 0;
	window_class.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOGO));
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)COLOR_WINDOW;
	window_class.lpszMenuName = NULL;
	window_class.lpfnWndProc = WindowProc;
#pragma endregion
#pragma region Window Creation
	nLength = LoadString(hInstance, IDS_WINDOW_CAPTION, (LPTSTR)&pDummy, 0);
	lpBuffer = HeapAlloc(GetProcessHeap(), 0, (nLength + 1) * sizeof(TCHAR));
	LoadString(hInstance, IDS_WINDOW_CAPTION, lpBuffer, nLength + 1);
	hWnd = CreateWindow(
		(LPCWSTR)RegisterClass(&window_class),
		lpBuffer,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		220,
		220,
		MIN_WIDTH,
		MIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	HeapFree(GetProcessHeap(), 0, lpBuffer);
#pragma endregion
#pragma region Message Loop
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
#pragma endregion 
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PVOID pDummy;
	HFONT hFont = (HFONT)GetWindowLongPtr(hWnd, 0);
	HANDLE hClientHandle = (HANDLE)GetWindowLongPtr(hWnd, sizeof(LONG_PTR));
	HWND hButton = (HWND)GetWindowLongPtr(hWnd, 2 * sizeof(LONG_PTR));
	DWORD dwNegotiatedVersion;
	WLAN_HOSTED_NETWORK_REASON failReason;
	PWLAN_HOSTED_NETWORK_STATUS pWlanHostedNetworkStatus;
	UINT uID;
	int nLength;
	RECT rect;
	LPTSTR lpBuffer;
	switch (uMsg) {
		case WM_CREATE:
#pragma region Initialize font
			hFont = CreateFont(
				48,
				0,
				0,
				0,
				400,
				0,
				0,
				0,
				1,
				0,
				0,
				0,
				0,
				TEXT("Segoe UI"));
			SetWindowLongPtr(hWnd, 0, (LONG_PTR)hFont);
#pragma endregion
			WlanOpenHandle(WLAN_API_VERSION, NULL, &dwNegotiatedVersion, &hClientHandle);
			SetWindowLongPtr(hWnd, sizeof(LONG_PTR), (LONG_PTR)hClientHandle);
			WlanHostedNetworkInitSettings(hClientHandle, &failReason, NULL);
			pWlanHostedNetworkStatus = NULL;
			WlanHostedNetworkQueryStatus(hClientHandle, &pWlanHostedNetworkStatus, NULL);
			switch (pWlanHostedNetworkStatus->HostedNetworkState) {
				case wlan_hosted_network_active:
					uID = IDS_HOTSPOT_RUNNING;
					break;
				case wlan_hosted_network_idle:
					uID = IDS_HOTSPOT_STOPPED;
					break;
				case wlan_hosted_network_unavailable:
				default:
					uID = IDS_ERROR;
			}
			WlanFreeMemory(pWlanHostedNetworkStatus);
			nLength = LoadString((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), uID, (LPTSTR)&pDummy, 0) + 1;
			lpBuffer = HeapAlloc(GetProcessHeap(), 0, (nLength) * sizeof(TCHAR));
			LoadString((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), uID, lpBuffer, nLength);
			GetClientRect(hWnd, &rect);
			hButton = CreateWindow(
				TEXT("BUTTON"),
				lpBuffer,
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				0,
				0,
				rect.right,
				rect.bottom,
				hWnd,
				NULL,
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
				NULL);
			SetWindowLongPtr(hWnd, 2 * sizeof(LONG_PTR), (LONG_PTR)hButton);
			HeapFree(GetProcessHeap(), 0, lpBuffer);
			SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, 0);
			return 0;
		case WM_COMMAND:
			if (lParam == (LPARAM)hButton) {
				pWlanHostedNetworkStatus = NULL;
				WlanHostedNetworkQueryStatus(hClientHandle, &pWlanHostedNetworkStatus, NULL);
				switch (pWlanHostedNetworkStatus->HostedNetworkState) {
					case wlan_hosted_network_active:
						WlanHostedNetworkForceStop(hClientHandle, &failReason, NULL);
						SendMessage(hButton, WM_SETTEXT, 0, IDS_HOTSPOT_STOPPED);
						break;
					case wlan_hosted_network_idle:
						WlanHostedNetworkForceStart(hClientHandle, &failReason, NULL);
						SendMessage(hButton, WM_SETTEXT, 0, IDS_HOTSPOT_RUNNING);
						break;
					case wlan_hosted_network_unavailable:
					default:
						SendMessage(hButton, WM_SETTEXT, 0, IDS_ERROR);
				}
				WlanFreeMemory(pWlanHostedNetworkStatus);
				return 0;
			}
			break;
		case WM_GETMINMAXINFO:
			((PMINMAXINFO)lParam)->ptMinTrackSize.x = MIN_WIDTH;
			((PMINMAXINFO)lParam)->ptMinTrackSize.y = MIN_HEIGHT;
			return 0;
		case WM_SIZE:
			if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED || wParam == SIZE_MAXSHOW) {
				GetClientRect(hWnd, &rect);
				MoveWindow(
					hButton,
					0,
					0,
					rect.right,
					rect.bottom,
					TRUE);
				return 0;
			}
			break;
		case WM_DESTROY:
			DeleteObject(hFont);
			WlanCloseHandle(hClientHandle,NULL);
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
