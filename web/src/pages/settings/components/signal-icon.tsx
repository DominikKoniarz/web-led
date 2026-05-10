import { SignalHigh, SignalLow, SignalMedium } from "lucide-react";

import { cn } from "@/lib/utils";

function getSignalStrength(rssi: number): "strong" | "medium" | "weak" {
    if (rssi > -50) return "strong";
    if (rssi > -70) return "medium";
    return "weak";
}

const icons = {
    strong: SignalHigh,
    medium: SignalMedium,
    weak: SignalLow,
};

const colors = {
    strong: "text-success",
    medium: "text-warning",
    weak: "text-destructive",
};

export default function SignalIcon({ rssi }: { rssi: number }) {
    const strength = getSignalStrength(rssi);

    const Icon = icons[strength];
    const color = colors[strength];

    return (
        <div className="relative h-4 w-4">
            <SignalHigh className="absolute inset-0 h-4 w-4 opacity-20" />
            <Icon className={cn("absolute inset-0 h-4 w-4", color)} />
        </div>
    );
}
