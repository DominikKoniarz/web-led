import { Button } from "@/components/ui/button";
import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import SystemInfoSkeleton from "@/pages/settings/components/system-info-skeleton";
import useDeviceInfo from "@/pages/settings/hooks/use-device-info";
import { formatBytes } from "@/pages/settings/lib/formatBytes";
import { formatUptime } from "@/pages/settings/lib/formatUptime";
import { Info, RotateCcw } from "lucide-react";

export default function SystemInfo() {
    const { data, error, isLoading } = useDeviceInfo();

    if (isLoading || !data) {
        return <SystemInfoSkeleton />;
    }
    if (error) {
        //todo error
        return null;
    }
    console.log(data);
    const systemInfoItems = [
        { label: "Firmware Version", value: "v1.0.0" },
        { label: "Chip", value: data?.chipModel ?? "N/A" },
        {
            label: "CPU Temperature",
            value: data?.cpuTempC ? `${data.cpuTempC.toFixed(1)} °C` : "N/A",
        },
        { label: "Uptime", value: formatUptime(data.uptimeSec) },
        { label: "Free memory", value: formatBytes(data.freeHeapBytes) },
    ];
    return (
        <Card>
            <CardHeader>
                <CardTitle className="flex items-center gap-2">
                    <Info className="text-primary h-5 w-5" />
                    System
                </CardTitle>
                <CardDescription>
                    System actions and information
                </CardDescription>
            </CardHeader>
            <CardContent className="space-y-4">
                <div className="bg-secondary/50 grid gap-2 rounded-lg p-4 text-sm">
                    {systemInfoItems.map((item, index) => (
                        <div
                            key={index}
                            className="flex items-center justify-between"
                        >
                            <span className="text-muted-foreground">
                                {item.label}
                            </span>
                            <span className="text-foreground font-mono">
                                {item.value}
                            </span>
                        </div>
                    ))}
                </div>

                <Button variant="outline" className="w-full cursor-pointer">
                    <RotateCcw className="mr-2 h-4 w-4" />
                    Restart Device
                </Button>
            </CardContent>
        </Card>
    );
}
