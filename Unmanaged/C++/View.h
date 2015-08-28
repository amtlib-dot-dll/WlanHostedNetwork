#pragma once
#include "stdafx.h"

namespace WlanHostedNetwork
{
	class View {
	protected:
		std::function<void(void)> _cbSwitch;
	public:
		virtual ~View() {};

		virtual void setSwitchCallback(decltype(_cbSwitch) cbSwitch) = 0;
		virtual void setState(WLAN_HOSTED_NETWORK_STATE state) = 0;
		virtual void show(int nCmdShow) = 0;
	};
}
