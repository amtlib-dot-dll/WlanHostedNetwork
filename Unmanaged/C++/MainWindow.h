#pragma once
#include "View.h"

namespace WlanHostedNetwork
{
	class MainWindow : public View {
		HWND _hMain;
		HWND _hSwitchButton;
		HFONT _hSegoeUI;
	public:
		MainWindow();
		~MainWindow();
		void setSwitchCallback(decltype(_cbSwitch) cbSwitch) override;
		void setState(WLAN_HOSTED_NETWORK_STATE state) override;
		void show(int nCmdShow) override;
	};
}
