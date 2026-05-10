import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import useLEDCount from "@/pages/settings/hooks/use-led-count";
import { Label } from "@radix-ui/react-label";

type Props = {
    ledCount: number;
};

export default function LEDCountSettings({ ledCount }: Props) {
    const {
        localLEDCount,
        ledMutation,
        isChanged,
        onLEDCountChange,
        saveLEDCount,
    } = useLEDCount(ledCount);

    return (
        <div className="space-y-2">
            <Label htmlFor="led-count">LED Count</Label>
            <Input
                id="led-count"
                type="number"
                min={1}
                max={120}
                onChange={(e) => onLEDCountChange(e.target.value)}
                value={localLEDCount.toString()}
                placeholder="Number of LEDs"
            />
            <p className="text-muted-foreground text-xs">
                Total number of LEDs in your strip (max is 120)
            </p>

            <div className="flex justify-end">
                <Button
                    className="bg-white"
                    disabled={!isChanged || ledMutation.isPending}
                    onClick={saveLEDCount}
                >
                    Save Settings
                </Button>
            </div>
        </div>
    );
}
