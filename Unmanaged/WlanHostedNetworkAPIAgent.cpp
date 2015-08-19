#include "WlanHostedNetworkAPIAgent.h"

namespace WlanHostedNetwork
{
	WlanHostedNetworkAPIAgent::WlanHostedNetworkAPIAgent() {
		DWORD dwNegotiatedVersion;
		WlanOpenHandle(WLAN_API_VERSION, nullptr, &dwNegotiatedVersion, &hClientHandle);
		WLAN_HOSTED_NETWORK_REASON failReason;
		WlanHostedNetworkInitSettings(hClientHandle, &failReason, nullptr);
	}

	WlanHostedNetworkAPIAgent::~WlanHostedNetworkAPIAgent() {
		WlanCloseHandle(hClientHandle, nullptr);
	}

	bool WlanHostedNetworkAPIAgent::isRunning() {
		PWLAN_HOSTED_NETWORK_STATUS pWlanHostedNetworkStatus = nullptr;
		WlanHostedNetworkQueryStatus(hClientHandle, &pWlanHostedNetworkStatus, nullptr);
		auto status = std::unique_ptr<WLAN_HOSTED_NETWORK_STATUS, std::function<void(PVOID)>>(pWlanHostedNetworkStatus, WlanFreeMemory);
		switch (status->HostedNetworkState) {
			case wlan_hosted_network_active:
				return true;
			case wlan_hosted_network_idle:
				return false;
			case wlan_hosted_network_unavailable:
			default:
				throw nullptr;
		}
	}

	void WlanHostedNetworkAPIAgent::start() {
		WLAN_HOSTED_NETWORK_REASON failReason;
		WlanHostedNetworkForceStart(hClientHandle, &failReason, nullptr);
	}

	void WlanHostedNetworkAPIAgent::stop() {
		WLAN_HOSTED_NETWORK_REASON failReason;
		WlanHostedNetworkForceStop(hClientHandle, &failReason, nullptr);
	}
}
