#include "stdafx.h"
#include "MainWindow.h"

namespace WlanHostedNetwork
{
	static const auto MIN_WIDTH = 768;
	static const auto MIN_HEIGHT = 128;

	MainWindow::MainWindow() {
		_hSegoeUI = CreateFont(
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
#pragma region Window Specification
		WNDCLASS window_class;
		window_class.lpszClassName = TEXT("WlanHostedNetwork") ;
		window_class.hInstance = GetModuleHandle(nullptr);
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.style = 0;
		window_class.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_LOGO));
		window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		window_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
		window_class.lpszMenuName = nullptr;
		window_class.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT {
			auto instance = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd,GWLP_USERDATA));
			RECT rect;
			switch (uMsg) {
				case WM_CREATE:
					instance = reinterpret_cast<MainWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
					SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(instance));
					GetClientRect(hWnd, &rect);
					instance->_hSwitchButton = CreateWindow(
						TEXT("BUTTON"),
						TEXT(""),
						WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
						0,
						0,
						rect.right ,
						rect.bottom ,
						hWnd,
						nullptr,
						GetModuleHandle(nullptr),
						nullptr);
					SendMessage(instance->_hSwitchButton, WM_SETFONT, reinterpret_cast<WPARAM>(instance->_hSegoeUI), 0);
					return 0;
				case WM_GETMINMAXINFO:
					reinterpret_cast<PMINMAXINFO>(lParam)->ptMinTrackSize = {MIN_WIDTH, MIN_HEIGHT};
					return 0;
				case WM_COMMAND:
					if (lParam == reinterpret_cast<LPARAM>(instance->_hSwitchButton)) {
						instance->_cbSwitch();
						return 0;
					}
					break;
				case WM_SIZE:
					if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED || wParam == SIZE_MAXSHOW) {
						GetClientRect(hWnd, &rect);
						MoveWindow(
							instance->_hSwitchButton,
							0,
							0,
							rect.right,
							rect.bottom,
							true);
						return 0;
					}
					break;
				case WM_DESTROY:
					PostQuitMessage(0);
					return 0;
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		};
#pragma endregion 
		LPCTSTR lpCaption;
		LoadString(GetModuleHandle(nullptr), IDS_WINDOW_CAPTION, reinterpret_cast<LPTSTR>(&lpCaption), 0);
		_hMain = CreateWindow(
			reinterpret_cast<LPCTSTR>(RegisterClass(&window_class)),
			lpCaption,
			WS_OVERLAPPEDWINDOW,
			220,
			220,
			MIN_WIDTH,
			MIN_HEIGHT,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			this);
	}

	MainWindow::~MainWindow() {
		DeleteObject(_hSegoeUI);
	}

	void MainWindow::setSwitchCallback(decltype(_cbSwitch) cbSwitch) {
		_cbSwitch = cbSwitch;
	}

	void MainWindow::setState(WLAN_HOSTED_NETWORK_STATE state) {
		LPCTSTR message;
		switch (state) {
			case wlan_hosted_network_active:
				LoadString(GetModuleHandle(nullptr), IDS_HOTSPOT_RUNNING, reinterpret_cast<LPTSTR>(&message), 0);
				break;
			case wlan_hosted_network_idle:
				LoadString(GetModuleHandle(nullptr), IDS_HOTSPOT_STOPPED, reinterpret_cast<LPTSTR>(&message), 0);
				break;
			case wlan_hosted_network_unavailable:
			default:
				LoadString(GetModuleHandle(nullptr), IDS_ERROR, reinterpret_cast<LPTSTR>(&message), 0);
				break;
		}
		SetWindowText(_hSwitchButton, message);
	}

	void MainWindow::show(int nCmdShow) {
		ShowWindow(_hMain, nCmdShow);
	}
}
