import LedControls from "@/pages/home/components/led-controls";
import { LEDModesPanel } from "@/pages/home/components/led-modes-panel";

export function HomeRouteComponent() {
    return (
        <div className="container mx-auto space-y-6 px-4 py-6">
            <LEDModesPanel />
            <LedControls />
        </div>
    );
}
