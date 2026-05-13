import { HomeSkeleton } from "@/pages/home/components/home-skeleton";
import LEDColor from "@/pages/home/components/led-color";
import LedControls from "@/pages/home/components/led-controls";
import { LEDModesPanel } from "@/pages/home/components/led-modes-panel";
import { useHomePageFetch } from "@/pages/home/hooks/use-home-page-fetch";

export function HomeRouteComponent() {
    const { ledState, ledStateError, isLedStateLoading } = useHomePageFetch();

    if (ledStateError) {
        return (
            <div className="container mx-auto px-4 py-6">
                <div className="rounded-xl border border-red-300 bg-red-50 p-4 text-red-700">
                    Unable to fetch data
                </div>
            </div>
        );
    }
    if (isLedStateLoading || !ledState) {
        return <HomeSkeleton />;
    }
    return (
        <div className="container mx-auto space-y-6 px-4 py-6">
            <LEDModesPanel mode={ledState.mode} />
            <LEDColor
                color={{ r: ledState.red, g: ledState.green, b: ledState.blue }}
            />
            <LedControls
                brightnessPercent={ledState.brightnessPercent}
                speedPercent={ledState.speedPercent}
            />
        </div>
    );
}
