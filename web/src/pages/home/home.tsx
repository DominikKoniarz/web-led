import LedControls from "@/pages/home/components/led-controls";
import { LEDModesPanel } from "@/pages/home/components/led-modes-panel";
import { useQuery } from "@tanstack/react-query";
import { useEffect } from "react";

type LedMode =
    | "off"
    | "solid"
    | "rainbow"
    | "breathing"
    | "chase"
    | "sparkle"
    | "fire"
    | "wave";

type LedState = {
    mode: LedMode;
    brightnessPercent: number;
    speedPercent: number;
    red: number;
    green: number;
    blue: number;
};

type SettingsState = {
    ledCount: number;
};

async function fetchJson<T>(path: string): Promise<T> {
    const response = await fetch(path, {
        headers: {
            Accept: "application/json",
        },
    });

    if (!response.ok) {
        const message = await response.text();
        throw new Error(`Request failed (${response.status}): ${message}`);
    }

    return (await response.json()) as T;
}

export function HomeRouteComponent() {
    const {
        data: ledState,
        error: ledStateError,
        isLoading: isLedStateLoading,
    } = useQuery({
        queryKey: ["leds"],
        queryFn: () => fetchJson<LedState>("http://webled.local/api/leds"),
    });

    const {
        data: settingsState,
        error: settingsStateError,
        isLoading: isSettingsLoading,
    } = useQuery({
        queryKey: ["settings"],
        queryFn: () =>
            fetchJson<SettingsState>("http://webled.local/api/settings"),
    });

    useEffect(() => {
        if (ledState) {
            console.log("LED state:", ledState);
        }

        if (ledStateError) {
            console.error("LED state error:", ledStateError);
        }
    }, [ledState, ledStateError]);

    useEffect(() => {
        if (settingsState) {
            console.log("Settings state:", settingsState);
        }

        if (settingsStateError) {
            console.error("Settings state error:", settingsStateError);
        }
    }, [settingsState, settingsStateError]);

    useEffect(() => {
        if (isLedStateLoading || isSettingsLoading) {
            console.log("Home data loading...", {
                isLedStateLoading,
                isSettingsLoading,
            });
        }
    }, [isLedStateLoading, isSettingsLoading]);

    return (
        <div className="container mx-auto space-y-6 px-4 py-6">
            <LEDModesPanel />
            <LedControls />
        </div>
    );
}
