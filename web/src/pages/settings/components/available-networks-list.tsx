import { cn } from "@/lib/utils";
import SignalIcon from "@/pages/settings/components/signal-icon";
import type { Network } from "@/pages/settings/types/wifi";
import { Lock, Unlock } from "lucide-react";

type Props = {
    networks: Network[];
};

export default function AvailableNetworksList({ networks }: Props) {
    return (
        <div className="space-y-2">
            {networks.map((network, index) => (
                <button
                    key={`${network.ssid}-${index}`}
                    type="button"
                    className={cn(
                        "flex w-full items-center justify-between rounded-lg border p-3 text-left transition-colors",
                        "hover:border-primary/50 hover:bg-secondary/50",
                    )}
                >
                    <div className="flex items-center gap-3">
                        <SignalIcon rssi={network.rssi} />
                        <span className="text-foreground font-medium">
                            {network.ssid}
                        </span>

                        {/* {connectedSsid === network.ssid && (
                            <Badge
                                variant="outline"
                                className="border-success/50 bg-success/10 text-success"
                            >
                                Connected
                            </Badge>
                        )} */}
                    </div>
                    <div className="flex items-center gap-2">
                        {network.auth !== "open" ? (
                            <Lock className="text-muted-foreground h-4 w-4" />
                        ) : (
                            <Unlock className="text-muted-foreground h-4 w-4" />
                        )}
                    </div>
                </button>
            ))}
        </div>
    );
}
