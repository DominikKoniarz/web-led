export type LEDMode =
    | "off"
    | "solid"
    | "rainbow"
    | "breathing"
    | "chase"
    | "sparkle"
    | "fire"
    | "wave";

export type LEDState = {
    mode: LEDMode;
    brightnessPercent: number;
    speedPercent: number;
    red: number;
    green: number;
    blue: number;
};

export type LEDOption = {
    mode: LEDMode;
    name: string;
    description: string;
    icon: string;
    color: string;
};
