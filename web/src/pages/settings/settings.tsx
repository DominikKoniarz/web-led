import { env } from "@/env";
import AvailableNetworks from "@/pages/settings/components/available-networks";
import DeviceSettings from "@/pages/settings/components/device-settings";
import SystemInfo from "@/pages/settings/components/system-info";
import type { WifiStatus } from "@/pages/settings/types/wifi";
import { useQuery } from "@tanstack/react-query";

export function SettingsRouteComponent() {
    const { data } = useQuery({
        queryKey: ["wifi-status"],
        queryFn: async () => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/wifi/status`,
                {
                    headers: {
                        Accept: "application/json",
                    },
                },
            );
            if (!response.ok) {
                const message = await response.text();
                throw new Error(
                    `Request failed (${response.status}): ${message}`,
                );
            }
            return (await response.json()) as WifiStatus;
        },
    });

    void data; // currently unused, remove later if used

    return (
        <div className="container mx-auto space-y-6 px-4 py-6">
            <AvailableNetworks />
            <DeviceSettings />
            <SystemInfo />
        </div>
    );
}
