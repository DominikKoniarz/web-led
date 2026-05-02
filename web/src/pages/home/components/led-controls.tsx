import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { Label } from "@/components/ui/label";
import { Slider } from "@/components/ui/slider";
import { useState } from "react";

export default function LedControls() {
    const [brightness, setBrightness] = useState([75]);
    const [speed, setSpeed] = useState([50]);
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
                            {brightness[0]}%
                        </span>
                    </div>
                    <Slider
                        id="brightness"
                        value={brightness}
                        onValueChange={setBrightness}
                        max={100}
                        step={1}
                        className="w-full"
                    />
                </div>

                <div className="space-y-3">
                    <div className="flex items-center justify-between">
                        <Label htmlFor="speed">Animation Speed</Label>
                        <span className="text-muted-foreground text-sm">
                            {speed[0]}%
                        </span>
                    </div>
                    <Slider
                        id="speed"
                        value={speed}
                        onValueChange={setSpeed}
                        max={100}
                        step={1}
                        className="w-full"
                    />
                </div>
            </CardContent>
        </Card>
    );
}
