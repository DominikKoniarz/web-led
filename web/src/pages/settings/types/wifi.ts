type WifiScanStartedResponse = {
    status: "started";
};

type WifiScanRunningResponse = {
    status: "running";
};

type WifiScanCompleteResponse = {
    status: "complete";
    count: number;
    networks: Network[];
};

export type WifiScanResponse =
    | WifiScanStartedResponse
    | WifiScanRunningResponse
    | WifiScanCompleteResponse;

type WifiAuth =
    | "open"
    | "wep"
    | "wpa_psk"
    | "wpa2_psk"
    | "wpa_wpa2_psk"
    | "wpa2_enterprise"
    | "wpa3_psk"
    | "wapi_psk"
    | "auth_owe"
    | "unknown";

export type Network = {
    ssid: string;
    rssi: number;
    channel: number;
    auth: WifiAuth;
};

type WifiStaStatus = {
    ssid: string;
    ip: string;
    rssi: number;
};

type WifiApStatus = {
    ssid: string;
    ip: string;
};

export type WifiStatus = {
    sta: WifiStaStatus;
    ap: WifiApStatus;
};
