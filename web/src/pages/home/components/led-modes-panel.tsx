import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { ledModes } from "@/lib/mock-data";
import { Palette } from "lucide-react";
import { useState } from "react";
import { LEDModeCard } from "./led-mode-card";

export function LEDModesPanel() {
    const [selectedMode, setSelectedMode] = useState<string>("solid");

    return (
        <Card>
            <CardHeader>
                <CardTitle className="flex items-center gap-2">
                    <Palette className="text-primary h-5 w-5" />
                    LED Modes
                </CardTitle>
                <CardDescription>
                    Select an effect for your LED strip
                </CardDescription>
            </CardHeader>
            <CardContent>
                <div className="grid grid-cols-2 gap-3 sm:grid-cols-3 md:grid-cols-4">
                    {ledModes.map((mode) => (
                        <LEDModeCard
                            key={mode.id}
                            mode={mode}
                            isSelected={selectedMode === mode.id}
                            onSelect={() => setSelectedMode(mode.id)}
                        />
                    ))}
                </div>
            </CardContent>
        </Card>
    );
}
