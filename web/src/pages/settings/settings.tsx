import { mockAvailableNetworks } from "@/lib/mock-data";
import AvailableNetworks from "@/pages/settings/components/available-networks";
import DeviceSettings from "@/pages/settings/components/device-settings";
import SystemInfo from "@/pages/settings/components/system-info";

export function SettingsRouteComponent() {
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
            <DeviceSettings />
            <SystemInfo />
        </div>
    );
}
