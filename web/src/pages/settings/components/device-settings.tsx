import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import DeviceSettingsSkeleton from "@/pages/settings/components/device-settings-skeleton";
import LEDCountSettings from "@/pages/settings/components/led-count-settings";
import useSettingsQuery from "@/pages/settings/hooks/use-settings-query";
import { Settings } from "lucide-react";

export default function DeviceSettings() {
    const { settingsState, settingsStateError, isSettingsLoading } =
        useSettingsQuery();

    if (isSettingsLoading || !settingsState) {
        return <DeviceSettingsSkeleton />;
    }
    if (settingsStateError) {
        //todo error
        return null;
    }

    return (
        <div>
            <Card>
                <CardHeader>
                    <CardTitle className="flex items-center gap-2">
                        <Settings className="text-primary h-5 w-5" />
                        Device Settings
                    </CardTitle>
                    <CardDescription>
                        Configure device LED count
                    </CardDescription>
                </CardHeader>
                <CardContent className="space-y-6">
                    <LEDCountSettings ledCount={settingsState.ledCount} />
                </CardContent>
            </Card>
        </div>
    );
}
