import { Button } from "@/components/ui/button";
import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { cn } from "@/lib/utils";
import AvailableNetworksList from "@/pages/settings/components/available-networks-list";
import AvailableNetworksListSkeleton from "@/pages/settings/components/available-networks-list-skeleton";
import ClickToScan from "@/pages/settings/components/click-to-scan";
import NetworkScanError from "@/pages/settings/components/network-scan-error";
import useWifiScan from "@/pages/settings/hooks/use-wifi-scan";
import { RefreshCw } from "lucide-react";

export default function AvailableNetworksCard() {
    const { data, error, isScanning, refetch } = useWifiScan();

    if (error) {
        return <NetworkScanError refetch={refetch} />;
    }

    return (
        <Card>
            <CardHeader>
                <div className="flex items-center justify-between">
                    <div>
                        <CardTitle>Available Networks</CardTitle>
                        <CardDescription>
                            Select a network to connect
                        </CardDescription>
                    </div>
                    <Button
                        variant="outline"
                        size="sm"
                        onClick={() => {
                            void refetch();
                        }}
                        disabled={isScanning}
                    >
                        <RefreshCw
                            className={cn(
                                "mr-2 h-4 w-4",
                                isScanning && "animate-spin",
                            )}
                        />
                        Scan
                    </Button>
                </div>
            </CardHeader>
            <CardContent>
                <div className="space-y-2">
                    {!data ? (
                        <ClickToScan />
                    ) : data.status === "started" ||
                      data.status === "running" ? (
                        <AvailableNetworksListSkeleton />
                    ) : (
                        <AvailableNetworksList networks={data.networks} />
                    )}
                </div>
            </CardContent>
        </Card>
    );
}
