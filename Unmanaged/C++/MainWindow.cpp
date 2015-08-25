#include "MainWindow.h"
#include "Utility.h"

namespace WlanHostedNetwork
{
	MainWindow::MainWindow() {
#pragma region Initialize font
		_font = CreateFont(
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
#pragma endregion 
#pragma region Initialize handlers
		addMessageHandler([](HWND hWnd, WPARAM wParam, LPARAM lParam)->bool {
			                  PostQuitMessage(0);
			                  return true;
		                  }, WM_DESTROY);
		addMessageHandler([](HWND hWnd, WPARAM wParam, LPARAM lParam)->bool {
			                  reinterpret_cast<PMINMAXINFO>(lParam)->ptMinTrackSize = {MIN_WIDTH, MIN_HEIGHT};
			                  return true;
		                  }, WM_GETMINMAXINFO);
#pragma endregion 
	}

	MainWindow::~MainWindow() {
		DeleteObject(_font);
	}

	MainWindow& MainWindow::getInstance() {
		static MainWindow instance;
		return instance;
	}

	void MainWindow::setHINSTANCE(HINSTANCE hInstance) {
		this->_hInstance = hInstance;
	}

	void MainWindow::addMessageHandler(std::function<bool(HWND hWnd, WPARAM wParam, LPARAM lParam)> WndProc, UINT uMsg) {
		_handlers.insert({uMsg, WndProc});
	}

	void MainWindow::create() {
#pragma region Window Specification
		WNDCLASS window_class;
		window_class.lpszClassName = TEXT("WlanHostedNetwork") ;
		window_class.hInstance = _hInstance;
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.style = 0;
		window_class.hIcon = LoadIcon(_hInstance, MAKEINTRESOURCE(IDI_ICON1));
		window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		window_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
		window_class.lpszMenuName = nullptr;
		window_class.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT {
			auto handled = false;
			auto tuple = getInstance()._handlers.equal_range(uMsg);
			for (; tuple.first != tuple.second; ++tuple.first) {
				handled &= tuple.first->second(hWnd, wParam, lParam);
			}
			return handled ? 0 : DefWindowProc(hWnd, uMsg, wParam, lParam);
		};
#pragma endregion 
		CreateWindow(
			reinterpret_cast<LPCTSTR>(RegisterClass(&window_class)),
			Utility::loadString(_hInstance, IDS_WINDOW_CAPTION).c_str(),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			220,
			220,
			MIN_WIDTH,
			MIN_HEIGHT,
			nullptr,
			nullptr,
			_hInstance,
			nullptr);
	}

	HFONT MainWindow::getFont() {
		return _font;
	}
}
