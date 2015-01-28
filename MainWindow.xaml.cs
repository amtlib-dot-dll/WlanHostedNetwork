using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows;

namespace WlanHostedNetwork {
    public partial class MainWindow : Window {
        private readonly IntPtr _handle;

        public MainWindow() {
            InitializeComponent();
            var fullPath = Process.GetCurrentProcess().MainModule.FileName;
            Title = fullPath.Substring(fullPath.LastIndexOf('\\') + 1, fullPath.LastIndexOf('.') - fullPath.LastIndexOf('\\') - 1);
            UInt32 pdwNegotiatedVersion;
            if (NativeHelper.WlanOpenHandle(2, IntPtr.Zero, out pdwNegotiatedVersion, out _handle) != 0) {
                Message.Text = "打开句柄失败，可能原因为网卡不正常";
            } else {
                if (Refresh()) {
                    return;
                }
            }
            Hint.Content = "点击关闭程序";
            Hint.Click += (sender, e) => Application.Current.Shutdown();
        }

        private bool Refresh() {
            NativeHelper.WLAN_HOSTED_NETWORK_STATUS status;
            if (NativeHelper.WlanHostedNetworkQueryStatus(_handle, out status, IntPtr.Zero) != 0) {
                Message.Text = "获取Wi-Fi热点信息失败";
                return false;
            }
            switch (status.HostedNetworkState) {
                case NativeHelper.WLAN_HOSTED_NETWORK_STATE.wlan_hosted_network_unavailable:
                    Message.Text = "Wi-Fi热点功能尚未启用";
                    return false;
                case NativeHelper.WLAN_HOSTED_NETWORK_STATE.wlan_hosted_network_idle:
                    Message.Text = "Wi-Fi热点已关闭";
                    Hint.Content = "点击以开启";
                    Hint.Click -= Stop;
                    Hint.Click += Start;
                    return true;
                case NativeHelper.WLAN_HOSTED_NETWORK_STATE.wlan_hosted_network_active:
                    Message.Text = "Wi-Fi热点已开启";
                    Hint.Content = "点击以关闭";
                    Hint.Click -= Start;
                    Hint.Click += Stop;
                    return true;
                default:
                    Message.Text = "未知错误";
                    return false;
            }
        }

        private void Stop(object sender, RoutedEventArgs e) {
            NativeHelper.WLAN_HOSTED_NETWORK_REASON reason;
            if (NativeHelper.WlanHostedNetworkForceStop(_handle, out reason, IntPtr.Zero) == 0 && reason == NativeHelper.WLAN_HOSTED_NETWORK_REASON.wlan_hosted_network_reason_success) {
                Refresh();
            }
        }

        private void Start(object sender, RoutedEventArgs e) {
            NativeHelper.WLAN_HOSTED_NETWORK_REASON reason;
            if (NativeHelper.WlanHostedNetworkForceStart(_handle, out reason, IntPtr.Zero) == 0 && reason == NativeHelper.WLAN_HOSTED_NETWORK_REASON.wlan_hosted_network_reason_success) {
                Refresh();
            }
        }
    }
    class NativeHelper {
        [DllImport("wlanapi.dll")]
        public extern static UInt32 WlanOpenHandle(
               [In] UInt32 dwClientVersion,
               IntPtr pReserved,
               [Out] out UInt32 pdwNegotiatedVersion,
               [Out] out IntPtr phClientHandle
           );

        [DllImport("wlanapi.dll")]
        public extern static UInt32 WlanCloseHandle(
            [In] IntPtr hClientHandle,
            IntPtr pReserved
        );

        [DllImport("wlanapi.dll")]
        public extern static UInt32 WlanHostedNetworkStartUsing(
            [In] IntPtr hClientHandle,
            [Out] out WLAN_HOSTED_NETWORK_REASON pFailReason,
            IntPtr pReserved
        );

        [DllImport("wlanapi.dll")]
        public extern static UInt32 WlanHostedNetworkStopUsing(
            [In] IntPtr hClientHandle,
            [Out] out WLAN_HOSTED_NETWORK_REASON pFailReason,
            IntPtr pReserved
        );

        [DllImport("wlanapi.dll")]
        public extern static UInt32 WlanHostedNetworkForceStart(
            [In] IntPtr hClientHandle,
            [Out] out WLAN_HOSTED_NETWORK_REASON pFailReason,
            IntPtr pReserved
        );
        [DllImport("wlanapi.dll")]
        public extern static UInt32 WlanHostedNetworkForceStop(
            [In] IntPtr hClientHandle,
            [Out] out WLAN_HOSTED_NETWORK_REASON pFailReason,
            IntPtr pReserved
        );
        [DllImport("wlanapi.dll")]
        public extern static UInt32 WlanHostedNetworkQueryStatus(
            [In] IntPtr hClientHandle,
            [Out] out WLAN_HOSTED_NETWORK_STATUS ppWlanHostedNetworkStatus,
            IntPtr pReserved
        );
        public enum WLAN_HOSTED_NETWORK_REASON {
            wlan_hosted_network_reason_success = 0,
            wlan_hosted_network_reason_unspecified,
            wlan_hosted_network_reason_bad_parameters,
            wlan_hosted_network_reason_service_shutting_down,
            wlan_hosted_network_reason_insufficient_resources,
            wlan_hosted_network_reason_elevation_required,
            wlan_hosted_network_reason_read_only,
            wlan_hosted_network_reason_persistence_failed,
            wlan_hosted_network_reason_crypt_error,
            wlan_hosted_network_reason_impersonation,
            wlan_hosted_network_reason_stop_before_start,

            wlan_hosted_network_reason_interface_available,
            wlan_hosted_network_reason_interface_unavailable,
            wlan_hosted_network_reason_miniport_stopped,
            wlan_hosted_network_reason_miniport_started,
            wlan_hosted_network_reason_incompatible_connection_started,
            wlan_hosted_network_reason_incompatible_connection_stopped,
            wlan_hosted_network_reason_user_action,
            wlan_hosted_network_reason_client_abort,
            wlan_hosted_network_reason_ap_start_failed,

            wlan_hosted_network_reason_peer_arrived,
            wlan_hosted_network_reason_peer_departed,
            wlan_hosted_network_reason_peer_timeout,
            wlan_hosted_network_reason_gp_denied,
            wlan_hosted_network_reason_service_unavailable,
            wlan_hosted_network_reason_device_change,
            wlan_hosted_network_reason_properties_change,
            wlan_hosted_network_reason_virtual_station_blocking_use,
            wlan_hosted_network_reason_service_available_on_virtual_station
        }
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public class WLAN_HOSTED_NETWORK_STATUS {
            //            public UInt32 HostedNetworkState;
            public WLAN_HOSTED_NETWORK_STATE HostedNetworkState;
            public Guid IPDeviceID;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public Byte[] wlanHostedNetworkBSSID;
            public DOT11_PHY_TYPE dot11PhyType;
            public UInt32 ulChannelFrequency;
            public UInt32 dwNumberOfPeers;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 1)]
            public WLAN_HOSTED_NETWORK_PEER_STATE[] PeerList;
        }
        public enum WLAN_HOSTED_NETWORK_STATE {
            wlan_hosted_network_unavailable,
            wlan_hosted_network_idle,
            wlan_hosted_network_active,
        }
        public enum DOT11_PHY_TYPE {
            dot11_phy_type_unknown = 0,
            dot11_phy_type_any = dot11_phy_type_unknown,
            dot11_phy_type_fhss = 1,
            dot11_phy_type_dsss = 2,
            dot11_phy_type_irbaseband = 3,
            dot11_phy_type_ofdm = 4,
            dot11_phy_type_hrdsss = 5,
            dot11_phy_type_erp = 6,
            dot11_phy_type_ht = 7,
            dot11_phy_type_vht = 8,
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct WLAN_HOSTED_NETWORK_PEER_STATE {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
            public Byte[] PeerMacAddress;
            public WLAN_HOSTED_NETWORK_PEER_AUTH_STATE PeerAuthState;
        }
        public enum WLAN_HOSTED_NETWORK_PEER_AUTH_STATE {
            wlan_hosted_network_peer_state_invalid,
            wlan_hosted_network_peer_state_authenticated,
        }
    }
}
