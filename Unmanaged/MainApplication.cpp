#include "stdafx.h"
#include "Utility.h"
#include "MainWindow.h"
#include "SwitchButton.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	using namespace WlanHostedNetwork;
	auto& window = MainWindow::getInstance();
	window.setHINSTANCE(hInstance);
	new SwitchButton();
	window.create();
	return static_cast<int>(Utility::messageLoop().wParam);
}
