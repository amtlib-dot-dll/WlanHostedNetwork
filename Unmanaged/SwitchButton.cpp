#include "SwitchButton.h"
#include "MainWindow.h"
#include "Utility.h"

namespace WlanHostedNetwork
{
	SwitchButton::SwitchButton() {
		MainWindow::getInstance().addMessageHandler(
			[this](HWND hWnd, WPARAM wParam, LPARAM lParam)->bool {
				RECT client_rect;
				GetClientRect(hWnd, &client_rect);
				_handle = CreateWindow(
					TEXT("BUTTON"),
					Utility::loadString(hWnd, _agent.isRunning() ? IDS_HOTSPOT_RUNNING : IDS_HOTSPOT_STOPPED).c_str(),
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
					client_rect.left,
					client_rect.top,
					client_rect.right - client_rect.left,
					client_rect.bottom - client_rect.top,
					hWnd,
					nullptr,
					reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE)),
					nullptr);
				SendMessage(_handle, WM_SETFONT, reinterpret_cast<WPARAM>(MainWindow::getInstance().getFont()), 0);
				return true;
			}, WM_CREATE);
		MainWindow::getInstance().addMessageHandler(
			[this](HWND hWnd, WPARAM wParam, LPARAM lParam)->bool {
				if (lParam != reinterpret_cast<LPARAM>(_handle)) {
					return false;
				}
				auto running = _agent.isRunning();
				if (running) {
					_agent.stop();
				} else {
					_agent.start();
				}
				SendMessage(_handle, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(Utility::loadString(hWnd, !running ? IDS_HOTSPOT_RUNNING : IDS_HOTSPOT_STOPPED).c_str()));
				return true;
			}, WM_COMMAND);
		MainWindow::getInstance().addMessageHandler(
			[this](HWND hWnd, WPARAM wParam, LPARAM lParam)->bool {
				if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED || wParam == SIZE_MAXSHOW) {
					RECT client_rect;
					GetClientRect(hWnd, &client_rect);
					MoveWindow(
						_handle,
						client_rect.left,
						client_rect.top,
						client_rect.right - client_rect.left,
						client_rect.bottom - client_rect.top,
						true);
					return true;
				} else {
					return false;
				}
			}, WM_SIZE);
	}
}
