#pragma once
#include "stdafx.h"
#include "WlanHostedNetworkAPIAgent.h"

namespace WlanHostedNetwork
{
	class SwitchButton {
		HWND _handle;
		WlanHostedNetworkAPIAgent _agent;
	public:
		SwitchButton();
	};
}
