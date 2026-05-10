import { Button } from "@/components/ui/button";
import {
    Dialog,
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
} from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import NetworkItem from "@/pages/settings/components/network-item";
import useWifiConnect from "@/pages/settings/hooks/use-wifi-connect";
import type { Network } from "@/pages/settings/types/wifi";
import { useState } from "react";

type Props = {
    networks: Network[];
};

export default function AvailableNetworksList({ networks }: Props) {
    const [selectedNetwork, setSelectedNetwork] = useState<Network | null>(
        null,
    );
    const [password, setPassword] = useState("");

    const { wifiConnect } = useWifiConnect();

    return (
        <div className="space-y-2">
            {networks.map((network, index) => (
                <>
                    <NetworkItem
                        key={`${network.ssid}-${index}`}
                        network={network}
                        index={index}
                        onSelect={() => setSelectedNetwork(network)}
                    />
                    <Dialog
                        open={selectedNetwork !== null}
                        onOpenChange={(open) => {
                            if (!open) setSelectedNetwork(null);
                        }}
                    >
                        <DialogContent>
                            <DialogHeader>
                                <DialogTitle>
                                    Connect to {selectedNetwork?.ssid}
                                </DialogTitle>
                                <DialogDescription>
                                    Enter the password for this network
                                </DialogDescription>
                            </DialogHeader>
                            <div className="space-y-4 py-4">
                                <div className="space-y-2">
                                    <Label htmlFor="wifi-password">
                                        Password
                                    </Label>
                                    <Input
                                        id="wifi-password"
                                        type="password"
                                        placeholder="Enter WiFi password"
                                        value={password}
                                        onChange={(e) =>
                                            setPassword(e.target.value)
                                        }
                                    />
                                </div>
                            </div>
                            <DialogFooter>
                                <Button
                                    variant="outline"
                                    onClick={() => setSelectedNetwork(null)}
                                >
                                    Cancel
                                </Button>
                                <Button
                                    onClick={() =>
                                        wifiConnect.mutate({
                                            ssid: network.ssid,
                                            password: password,
                                        })
                                    }
                                    disabled={!password}
                                >
                                    Connect
                                </Button>
                            </DialogFooter>
                        </DialogContent>
                    </Dialog>
                </>
            ))}
        </div>
    );
}
