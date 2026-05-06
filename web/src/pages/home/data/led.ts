import type { LEDOption } from "@/pages/home/types/led";

export const ledOptions: LEDOption[] = [
    {
        mode: "solid",
        name: "Solid Color",
        description: "Single static color",
        icon: "circle",
        color: "#10b981",
    },
    {
        mode: "rainbow",
        name: "Rainbow",
        description: "Smooth rainbow cycle",
        icon: "rainbow",
        color: "#f59e0b",
    },
    {
        mode: "breathing",
        name: "Breathing",
        description: "Gentle pulse effect",
        icon: "wind",
        color: "#3b82f6",
    },
    {
        mode: "chase",
        name: "Chase",
        description: "Running light effect",
        icon: "zap",
        color: "#8b5cf6",
    },
    {
        mode: "sparkle",
        name: "Sparkle",
        description: "Random twinkling lights",
        icon: "sparkles",
        color: "#ec4899",
    },
    {
        mode: "fire",
        name: "Fire",
        description: "Flickering flame effect",
        icon: "flame",
        color: "#ef4444",
    },
    {
        mode: "wave",
        name: "Wave",
        description: "Flowing wave pattern",
        icon: "waves",
        color: "#06b6d4",
    },
    {
        mode: "off",
        name: "Off",
        description: "Turn off all LEDs",
        icon: "power",
        color: "#6b7280",
    },
];
