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
import { cn } from "@/lib/utils";
import SignalIcon from "@/pages/settings/components/signal-icon";
import useWifiConnect from "@/pages/settings/hooks/use-wifi-connect";
import type { Network } from "@/pages/settings/types/wifi";
import { Lock, Unlock } from "lucide-react";
import { useState } from "react";

type NetworkItemProps = {
    network: Network;
};

export default function NetworkItem({ network }: NetworkItemProps) {
    const [open, setOpen] = useState(false);

    const [password, setPassword] = useState("");

    const { wifiConnect } = useWifiConnect();

    // TODO: add a toast after successful connection attempts

    return (
        <>
            <Dialog open={open} onOpenChange={setOpen}>
                <DialogContent>
                    <DialogHeader>
                        <DialogTitle>Connect to {network.ssid}</DialogTitle>
                        <DialogDescription>
                            Enter the password for this network
                        </DialogDescription>
                    </DialogHeader>
                    <div className="space-y-4 py-4">
                        <div className="space-y-2">
                            <Label htmlFor="wifi-password">Password</Label>
                            <Input
                                id="wifi-password"
                                type="password"
                                placeholder="Enter WiFi password"
                                value={password}
                                onChange={(e) => setPassword(e.target.value)}
                            />
                        </div>
                    </div>
                    <DialogFooter>
                        <Button
                            variant="outline"
                            onClick={() => setOpen(false)}
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
            <button
                onClick={() => setOpen(true)}
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
        </>
    );
}
