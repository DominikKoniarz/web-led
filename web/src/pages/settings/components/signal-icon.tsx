import { cn } from "@/lib/utils";
import { Signal } from "lucide-react";

function getSignalStrength(rssi: number): "strong" | "medium" | "weak" {
    if (rssi > -50) return "strong";
    if (rssi > -70) return "medium";
    return "weak";
}

export default function SignalIcon({ rssi }: { rssi: number }) {
    const strength = getSignalStrength(rssi);
    return (
        <Signal
            className={cn(
                "h-4 w-4",
                strength === "strong" && "text-success",
                strength === "medium" && "text-warning",
                strength === "weak" && "text-destructive",
            )}
        />
    );
}
