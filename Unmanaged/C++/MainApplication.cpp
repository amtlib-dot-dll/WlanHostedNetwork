#include "stdafx.h"
#include "Utility.h"
#include "MainWindow.h"
#include "SwitchButton.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	auto& window = WlanHostedNetwork::MainWindow::getInstance();
	window.setHINSTANCE(hInstance);
	new WlanHostedNetwork::SwitchButton();
	window.create();
	return static_cast<int>(WlanHostedNetwork::Utility::messageLoop().wParam);
}
