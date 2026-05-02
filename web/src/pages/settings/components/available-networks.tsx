import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { cn } from "@/lib/utils";
import { Lock, RefreshCw, Signal, Unlock } from "lucide-react";

export interface WifiNetwork {
    ssid: string;
    rssi: number;
    secure: boolean;
}

interface AvailableNetworksCardProps {
    networks: WifiNetwork[];
    connectedSsid?: string | null;
    isScanning?: boolean;
    onScan?: () => void;
    onSelectNetwork?: (network: WifiNetwork) => void;
}

export default function AvailableNetworksCard({
    networks,
    connectedSsid,
    isScanning = false,
    onScan,
    onSelectNetwork,
}: AvailableNetworksCardProps) {
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
                        onClick={onScan}
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
                    {networks.map((network) => (
                        <button
                            key={network.ssid}
                            type="button"
                            onClick={() => onSelectNetwork?.(network)}
                            className={cn(
                                "flex w-full items-center justify-between rounded-lg border p-3 text-left transition-colors",
                                "hover:border-primary/50 hover:bg-secondary/50",
                            )}
                        >
                            <div className="flex items-center gap-3">
                                <Signal className="text-success h-4 w-4" />
                                <span className="text-foreground font-medium">
                                    {network.ssid}
                                </span>

                                {connectedSsid === network.ssid && (
                                    <Badge
                                        variant="outline"
                                        className="border-success/50 bg-success/10 text-success"
                                    >
                                        Connected
                                    </Badge>
                                )}
                            </div>

                            <div className="flex items-center gap-2">
                                {network.secure ? (
                                    <Lock className="text-muted-foreground h-4 w-4" />
                                ) : (
                                    <Unlock className="text-muted-foreground h-4 w-4" />
                                )}
                            </div>
                        </button>
                    ))}
                </div>
            </CardContent>
        </Card>
    );
}
