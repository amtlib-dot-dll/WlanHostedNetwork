#include "stdafx.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	static NONCLIENTMETRICS non_client_metrics;
	non_client_metrics.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &non_client_metrics, 0);
	static auto font_handle = CreateFontIndirect(&non_client_metrics.lfStatusFont);
	WNDCLASS window_class;
	window_class.lpszClassName = TEXT("WlanHostedNetwork") ;
	window_class.hInstance = hInstance;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.style = 0;
	window_class.hIcon = LoadIcon(nullptr, IDI_EXCLAMATION);
	window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	window_class.lpszMenuName = nullptr;
	window_class.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT {
		static HWND button_handle;
		static HWND edit_handle;
		switch (uMsg) {
			case WM_CREATE: {
				LPTSTR pool = nullptr;
				std::wstring string(pool, LoadString(reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE)), IDS_STRING103, reinterpret_cast<LPTSTR>(&pool), 0));
				button_handle = CreateWindow(
					TEXT("BUTTON"),
					string.c_str(),
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
					10,
					40,
					700,
					non_client_metrics.iCaptionHeight,
					hWnd,
					nullptr,
					reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE)),
					nullptr);
				SendMessage(button_handle, WM_SETFONT, reinterpret_cast<WPARAM>(font_handle), 0);
				edit_handle = CreateWindow(
					TEXT("EDIT"),
					nullptr,
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					10,
					10,
					700,
					non_client_metrics.iCaptionHeight,
					hWnd,
					nullptr,
					reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE)),
					nullptr);
				SendMessage(edit_handle, WM_SETFONT, reinterpret_cast<WPARAM>(font_handle), 0);
				return 0;
			}
			case WM_COMMAND:
				if (lParam == reinterpret_cast<LPARAM>(button_handle)) {
					DWORD dwNegotiatedVersion;
					HANDLE hClientHandle;
					WlanOpenHandle(WLAN_API_VERSION, nullptr, &dwNegotiatedVersion, &hClientHandle);
					WLAN_HOSTED_NETWORK_REASON failReason;
					WlanHostedNetworkInitSettings(hClientHandle, &failReason, nullptr);
					DWORD dwKeyLength;
					UCHAR* pucKeyData;
					BOOL bIsPassPhrase;
					BOOL bPersistent;
					WlanHostedNetworkQuerySecondaryKey(
						hClientHandle,
						&dwKeyLength,
						&pucKeyData,
						&bIsPassPhrase,
						&bPersistent,
						&failReason,
						nullptr);
					if (bIsPassPhrase) {
						auto length = MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCCH>(pucKeyData), -1, nullptr, 0);
						auto buffer = std::make_unique<WCHAR[]>(length);
						MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCCH>(pucKeyData), -1, buffer.get(), length);
						SendMessage(edit_handle, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(buffer.get()));
						WlanFreeMemory(pucKeyData);
						WlanCloseHandle(hClientHandle, nullptr);
					} else {
						LPTSTR pool = nullptr;
						std::wstring string(pool, LoadString(reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE)), IDS_STRING102, reinterpret_cast<LPTSTR>(&pool), 0));
						SendMessage(edit_handle, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(string.c_str()));
					}
					return 0;
				} else {
					return DefWindowProc(hWnd, uMsg, wParam, lParam);
				}
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	};
	LPTSTR pool = nullptr;
	std::wstring string(pool, LoadString(hInstance, IDS_STRING101, reinterpret_cast<LPTSTR>(&pool), 0));
	CreateWindow(
		reinterpret_cast<LPCTSTR>(RegisterClass(&window_class)),
		string.c_str(),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_MINIMIZEBOX,
		220,
		220,
		750,
		350,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(font_handle);
	return static_cast<int>(msg.wParam);
}
