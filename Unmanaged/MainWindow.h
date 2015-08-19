#pragma once
#include "stdafx.h"

namespace WlanHostedNetwork
{
	class MainWindow {
		HFONT _font;
		HINSTANCE _hInstance;
		std::unordered_map<UINT, std::function<bool(HWND hWnd, WPARAM wParam, LPARAM lParam)>> _handlers;
		explicit MainWindow();
		MainWindow(MainWindow const&) = delete;
		void operator=(MainWindow const&) = delete;
	public:
		~MainWindow();
		static MainWindow& getInstance();
		void setHINSTANCE(HINSTANCE hInstance);
		void addMessageHandler(std::function<bool(HWND hWnd, WPARAM wParam, LPARAM lParam)> WndProc, UINT uMsg);
		void create();
		decltype(_font) getFont();
	};
}
