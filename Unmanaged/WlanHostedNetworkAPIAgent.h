#pragma once
#include "stdafx.h"

namespace WlanHostedNetwork
{
	class WlanHostedNetworkAPIAgent {
		HANDLE hClientHandle;
	public:
		WlanHostedNetworkAPIAgent();
		~WlanHostedNetworkAPIAgent();
		bool isRunning();
		void start();
		void stop();
	};
}
