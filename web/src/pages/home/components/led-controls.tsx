import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { Label } from "@/components/ui/label";
import { Slider } from "@/components/ui/slider";
import useBrightness from "@/pages/home/hooks/use-brightness";
import useSpeed from "@/pages/home/hooks/use-speed";

type Props = {
    brightnessPercent: number;
    speedPercent: number;
};

export default function LedControls({
    brightnessPercent: initialBrightnessPercent,
    speedPercent: initialSpeedPercent,
}: Props) {
    const { onBrightnessValueChange, brightnessPercent } = useBrightness(
        initialBrightnessPercent,
    );
    const { onSpeedValueChange, speedPercent } = useSpeed(initialSpeedPercent);

    return (
        <Card>
            <CardHeader>
                <CardTitle>Controls</CardTitle>
                <CardDescription>
                    Adjust brightness and animation speed
                </CardDescription>
            </CardHeader>
            <CardContent className="space-y-6">
                <div className="space-y-3">
                    <div className="flex items-center justify-between">
                        <Label htmlFor="brightness">Brightness</Label>
                        <span className="text-muted-foreground text-sm">
                            {brightnessPercent}%
                        </span>
                    </div>
                    <Slider
                        id="brightness"
                        value={[brightnessPercent]}
                        onValueChange={(value) =>
                            onBrightnessValueChange(value[0])
                        }
                        max={100}
                        step={1}
                        className="w-full"
                    />
                </div>

                <div className="space-y-3">
                    <div className="flex items-center justify-between">
                        <Label htmlFor="speed">Animation Speed</Label>
                        <span className="text-muted-foreground text-sm">
                            {speedPercent}%
                        </span>
                    </div>
                    <Slider
                        id="speed"
                        value={[speedPercent]}
                        onValueChange={(value) => onSpeedValueChange(value[0])}
                        max={100}
                        step={1}
                        className="w-full"
                    />
                </div>
            </CardContent>
        </Card>
    );
}
