#include "stdafx.h"
#include "MainWindow.h"
#include "WlanAPIAgent.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	WlanHostedNetwork::View* view = new WlanHostedNetwork::MainWindow();
	auto model = new WlanHostedNetwork::WlanAPIAgent();
	view->setState(model->getRunningState());
	view->setSwitchCallback([view,model]() {
		switch (model->getRunningState()) {
			case wlan_hosted_network_idle:
				model->start();
				break;
			case wlan_hosted_network_active:
				model->stop();
				break;
			default:
				throw nullptr;
		}
		view->setState(model->getRunningState());
	});
	view->show(nShowCmd);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}
