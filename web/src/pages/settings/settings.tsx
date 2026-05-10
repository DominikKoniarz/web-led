import { env } from "@/env";
import { mockAvailableNetworks } from "@/lib/mock-data";
import AvailableNetworks from "@/pages/settings/components/available-networks";
import DeviceSettings from "@/pages/settings/components/device-settings";
import SettingsSkeleton from "@/pages/settings/components/settings-skeleton";
import SystemInfo from "@/pages/settings/components/system-info";
import type { SettingsState } from "@/pages/settings/types/settings-state";
import { useQuery } from "@tanstack/react-query";

export function SettingsRouteComponent() {
    const {
        data: settingsState,
        error: settingsStateError,
        isLoading: isSettingsLoading,
    } = useQuery({
        queryKey: ["settings"],
        queryFn: async () => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/settings`,
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
            return (await response.json()) as SettingsState;
        },
    });
    if (isSettingsLoading || !settingsState) {
        return <SettingsSkeleton />;
    }
    if (settingsStateError) {
        //todo error
        return;
    }

    return (
        <div className="container mx-auto space-y-6 px-4 py-6">
            <AvailableNetworks
                networks={mockAvailableNetworks}
                connectedSsid="MyHomeNetwork"
                isScanning={false}
                onScan={() => {}}
                onSelectNetwork={(network) => {
                    console.log("selected:", network);
                }}
            />
            <DeviceSettings ledCount={settingsState.ledCount} />
            <SystemInfo />
        </div>
    );
}
