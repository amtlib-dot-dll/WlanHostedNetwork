#pragma once
#include "stdafx.h"

namespace WlanHostedNetwork
{
	const auto MIN_WIDTH = 768;
	const auto MIN_HEIGHT = 128;

	namespace Utility
	{
		std::wstring loadString(HINSTANCE hInstance, UINT uID);
		std::wstring loadString(HWND hWnd, UINT uID);
		HINSTANCE getHINSTANCE(HWND hWnd);
		MSG messageLoop();

		inline std::wstring loadString(HINSTANCE hInstance, UINT uID) {
			LPTSTR pool = nullptr ;
			return std::wstring(pool, LoadString(hInstance, uID, reinterpret_cast<LPTSTR>(&pool), 0));
		}

		inline std::wstring loadString(HWND hWnd, UINT uID) {
			return loadString(getHINSTANCE(hWnd), uID);
		}

		inline HINSTANCE getHINSTANCE(HWND hWnd) {
			return reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));
		}

		inline MSG messageLoop() {
			MSG msg;
			while (GetMessage(&msg, nullptr, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return msg;
		}
	};
}
