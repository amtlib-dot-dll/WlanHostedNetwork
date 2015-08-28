#pragma once
#include "stdafx.h"

namespace WlanHostedNetwork
{
	class WlanAPIAgent {
		HANDLE _hClientHandle;
	public:
		WlanAPIAgent();
		~WlanAPIAgent();
		WLAN_HOSTED_NETWORK_STATE getRunningState();
		void start();
		void stop();
	};
}
