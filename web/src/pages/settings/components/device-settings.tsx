import { Button } from "@/components/ui/button";
import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import useLEDCount from "@/pages/settings/hooks/use-led-count";
import { Settings } from "lucide-react";

type Props = {
    ledCount: number;
};

export default function DeviceSettings({ ledCount }: Props) {
    const {
        localLEDCount,
        ledMutation,
        isChanged,
        onLEDCountChange,
        saveLEDCount,
    } = useLEDCount(ledCount);
    return (
        <Card>
            <CardHeader>
                <CardTitle className="flex items-center gap-2">
                    <Settings className="text-primary h-5 w-5" />
                    Device Settings
                </CardTitle>
                <CardDescription>Configure device LED count</CardDescription>
            </CardHeader>
            <CardContent className="space-y-6">
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
                </div>
                <div className="flex justify-end">
                    <Button
                        className="bg-white"
                        disabled={!isChanged || ledMutation.isPending}
                        onClick={saveLEDCount}
                    >
                        Save Settings
                    </Button>
                </div>
            </CardContent>
        </Card>
    );
}
