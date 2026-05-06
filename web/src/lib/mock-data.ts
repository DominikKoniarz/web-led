export interface NetworkConfig {
    mode: "ap" | "station";
    ssid: string;
    connected: boolean;
    ipAddress: string;
    gateway: string;
    subnet: string;
    useStaticIP: boolean;
    apSSID?: string;
}

export interface WifiNetwork {
    ssid: string;
    rssi: number;
    secure: boolean;
}

export const mockNetworkConfigAP: NetworkConfig = {
    mode: "ap",
    ssid: "",
    connected: false,
    ipAddress: "192.168.4.1",
    gateway: "192.168.4.1",
    subnet: "255.255.255.0",
    useStaticIP: false,
    apSSID: "WebLED-AP",
};

export const mockNetworkConfigStation: NetworkConfig = {
    mode: "station",
    ssid: "MyHomeNetwork",
    connected: true,
    ipAddress: "192.168.1.105",
    gateway: "192.168.1.1",
    subnet: "255.255.255.0",
    useStaticIP: false,
};

export const mockAvailableNetworks: WifiNetwork[] = [
    { ssid: "MyHomeNetwork", rssi: -45, secure: true },
    { ssid: "Neighbors_WiFi", rssi: -67, secure: true },
    { ssid: "CoffeeShop", rssi: -72, secure: false },
    { ssid: "Office_5G", rssi: -58, secure: true },
];
