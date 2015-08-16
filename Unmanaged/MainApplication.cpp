#include "stdafx.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	static NONCLIENTMETRICS non_client_metrics;
	non_client_metrics.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &non_client_metrics, 0);
	static auto font_handle = CreateFontIndirect(&non_client_metrics.lfStatusFont);
	DWORD dwNegotiatedVersion;
	static HANDLE hClientHandle;
	WlanOpenHandle(WLAN_API_VERSION, nullptr, &dwNegotiatedVersion, &hClientHandle);
	static WLAN_HOSTED_NETWORK_REASON failReason;
	WlanHostedNetworkInitSettings(hClientHandle, &failReason, nullptr);
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
		switch (uMsg) {
			case WM_CREATE: {
				PWLAN_HOSTED_NETWORK_STATUS pWlanHostedNetworkStatus = nullptr;
				WlanHostedNetworkQueryStatus(hClientHandle, &pWlanHostedNetworkStatus, nullptr);
				UINT uID;
				switch (pWlanHostedNetworkStatus->HostedNetworkState) {
					case wlan_hosted_network_active:
						uID = IDS_HOTSPOT_RUNNING;
						break;
					case wlan_hosted_network_idle:
						uID = IDS_HOTSPOT_STOPPED;
						break;
					case wlan_hosted_network_unavailable:
						uID = IDS_HOTSPOT_ERROR;
						break;
				}
				LPTSTR pool = nullptr;
				std::wstring string(pool, LoadString(reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE)), uID, reinterpret_cast<LPTSTR>(&pool), 0));
				RECT client_rect;
				GetClientRect(hWnd, &client_rect);
				button_handle = CreateWindow(
					TEXT("BUTTON"),
					string.c_str(),
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
					client_rect.left,
					client_rect.top,
					client_rect.right - client_rect.left,
					client_rect.bottom - client_rect.top,
					hWnd,
					nullptr,
					reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE)),
					nullptr);
				SendMessage(button_handle, WM_SETFONT, reinterpret_cast<WPARAM>(font_handle), 0);
				if (uID == IDS_HOTSPOT_ERROR) {
					EnableWindow(button_handle, false);
				}
				WlanFreeMemory(pWlanHostedNetworkStatus);
				return 0;
			}
			case WM_COMMAND:
				if (lParam == reinterpret_cast<LPARAM>(button_handle)) {
					PWLAN_HOSTED_NETWORK_STATUS pWlanHostedNetworkStatus = nullptr;
					WlanHostedNetworkQueryStatus(hClientHandle, &pWlanHostedNetworkStatus, nullptr);
					UINT uID;
					switch (pWlanHostedNetworkStatus->HostedNetworkState) {
						case wlan_hosted_network_active:
							uID = IDS_HOTSPOT_STOPPED;
							WlanHostedNetworkForceStop(hClientHandle, &failReason, nullptr);
							break;
						case wlan_hosted_network_idle:
							uID = IDS_HOTSPOT_RUNNING;
							WlanHostedNetworkForceStart(hClientHandle, &failReason, nullptr);
							break;
						case wlan_hosted_network_unavailable:
							uID = IDS_HOTSPOT_ERROR;
							EnableWindow(button_handle, false);
							break;
					}
					LPTSTR pool = nullptr;
					std::wstring string(pool, LoadString(reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE)), uID, reinterpret_cast<LPTSTR>(&pool), 0));
					SendMessage(button_handle, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(string.c_str()));
					WlanFreeMemory(pWlanHostedNetworkStatus);
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
	std::wstring string(pool, LoadString(hInstance, IDS_WINDOW_CAPTION, reinterpret_cast<LPTSTR>(&pool), 0));
	CreateWindow(
		reinterpret_cast<LPCTSTR>(RegisterClass(&window_class)),
		string.c_str(),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_MINIMIZEBOX,
		220,
		220,
		750,
		450,
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
	WlanCloseHandle(hClientHandle, nullptr);
	return static_cast<int>(msg.wParam);
}
