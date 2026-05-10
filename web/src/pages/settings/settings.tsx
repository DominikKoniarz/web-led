import AvailableNetworks from "@/pages/settings/components/available-networks";
import DeviceSettings from "@/pages/settings/components/device-settings";
import SystemInfo from "@/pages/settings/components/system-info";

export function SettingsRouteComponent() {
    return (
        <div className="container mx-auto space-y-6 px-4 py-6">
            <AvailableNetworks />
            <DeviceSettings />
            <SystemInfo />
        </div>
    );
}
