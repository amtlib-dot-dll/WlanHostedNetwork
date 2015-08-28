#include "stdafx.h"
#include "WlanAPIAgent.h"

namespace WlanHostedNetwork
{
	WlanAPIAgent::WlanAPIAgent() {
		DWORD dwNegotiatedVersion;
		WlanOpenHandle(WLAN_API_VERSION, nullptr, &dwNegotiatedVersion, &_hClientHandle);
		WLAN_HOSTED_NETWORK_REASON failReason;
		WlanHostedNetworkInitSettings(_hClientHandle, &failReason, nullptr);
	}

	WlanAPIAgent::~WlanAPIAgent() {
		WlanCloseHandle(_hClientHandle, nullptr);
	}

	WLAN_HOSTED_NETWORK_STATE WlanAPIAgent::getRunningState() {
		PWLAN_HOSTED_NETWORK_STATUS pWlanHostedNetworkStatus = nullptr;
		WlanHostedNetworkQueryStatus(_hClientHandle, &pWlanHostedNetworkStatus, nullptr);
		auto state = pWlanHostedNetworkStatus->HostedNetworkState;
		WlanFreeMemory(pWlanHostedNetworkStatus);
		return state;
	}

	void WlanAPIAgent::start() {
		WLAN_HOSTED_NETWORK_REASON failReason;
		WlanHostedNetworkForceStart(_hClientHandle, &failReason, nullptr);
	}

	void WlanAPIAgent::stop() {
		WLAN_HOSTED_NETWORK_REASON failReason;
		WlanHostedNetworkForceStop(_hClientHandle, &failReason, nullptr);
	}
}
