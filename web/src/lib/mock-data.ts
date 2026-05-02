export interface LEDMode {
    id: string;
    name: string;
    description: string;
    icon: string;
    color?: string;
}

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

export const ledModes: LEDMode[] = [
    {
        id: "solid",
        name: "Solid Color",
        description: "Single static color",
        icon: "circle",
        color: "#10b981",
    },
    {
        id: "rainbow",
        name: "Rainbow",
        description: "Smooth rainbow cycle",
        icon: "rainbow",
        color: "#f59e0b",
    },
    {
        id: "breathing",
        name: "Breathing",
        description: "Gentle pulse effect",
        icon: "wind",
        color: "#3b82f6",
    },
    {
        id: "chase",
        name: "Chase",
        description: "Running light effect",
        icon: "zap",
        color: "#8b5cf6",
    },
    {
        id: "sparkle",
        name: "Sparkle",
        description: "Random twinkling lights",
        icon: "sparkles",
        color: "#ec4899",
    },
    {
        id: "fire",
        name: "Fire",
        description: "Flickering flame effect",
        icon: "flame",
        color: "#ef4444",
    },
    {
        id: "wave",
        name: "Wave",
        description: "Flowing wave pattern",
        icon: "waves",
        color: "#06b6d4",
    },
    {
        id: "off",
        name: "Off",
        description: "Turn off all LEDs",
        icon: "power",
        color: "#6b7280",
    },
];

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
