import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import NetworkStatusSkeleton from "@/pages/settings/components/network-status-skeleton";
import useNetworkStatus from "@/pages/settings/hooks/use-network-status";

export default function NetworkStatus() {
    const { wifiStatus, error, isLoading } = useNetworkStatus();

    if (isLoading) {
        return <NetworkStatusSkeleton />;
    }
    if (!wifiStatus || error) {
        return (
            <Card>
                <CardHeader>
                    <CardTitle className="flex items-center gap-2">
                        Network Status
                    </CardTitle>
                </CardHeader>
                <CardContent className="space-y-4">
                    <div className="space-y-4">
                        <div className="bg-secondary/50 grid gap-2 rounded-lg p-4">
                            <h1 className="text-red-500">
                                Failed to load network status
                            </h1>
                        </div>
                    </div>
                </CardContent>
            </Card>
        );
    }

    const items = [
        {
            title: "Access Point Mode",
            status: wifiStatus.ap,
        },
        {
            title: "Station Mode",
            status: wifiStatus.sta,
        },
    ];
    return (
        <Card>
            <CardHeader>
                <CardTitle className="flex items-center gap-2">
                    Network Status
                </CardTitle>
            </CardHeader>
            <CardContent className="space-y-4">
                <div className="space-y-4">
                    {items.map((item) => (
                        <div
                            key={item.title}
                            className="bg-secondary/50 grid gap-2 rounded-lg p-4"
                        >
                            <h1>{item.title}</h1>
                            <div className="flex items-center justify-between">
                                <span className="text-muted-foreground text-sm">
                                    Network Name
                                </span>
                                <span className="text-foreground font-mono text-sm">
                                    {item.status?.ssid ?? "Not connected"}
                                </span>
                            </div>
                            <div className="flex items-center justify-between">
                                <span className="text-muted-foreground text-sm">
                                    IP Address
                                </span>
                                <span className="text-foreground font-mono text-sm">
                                    {item.status?.ip ?? "N/A"}
                                </span>
                            </div>
                        </div>
                    ))}
                </div>
            </CardContent>
        </Card>
    );
}
