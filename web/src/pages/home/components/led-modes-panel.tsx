import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { LEDModeCard } from "@/pages/home/components/led-mode-card";
import { ledOptions } from "@/pages/home/data/led";
import useLEDModesPanel from "@/pages/home/hooks/use-led-modes-panel";
import type { LEDMode } from "@/pages/home/types/led";
import { Palette } from "lucide-react";

type Props = {
    mode: LEDMode;
};

export function LEDModesPanel({ mode }: Props) {
    const { onSelect, selectedMode } = useLEDModesPanel(mode);

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
                    {ledOptions.map((option) => (
                        <LEDModeCard
                            key={option.mode}
                            mode={option}
                            isSelected={selectedMode === option.mode}
                            onSelect={() => onSelect(option.mode)}
                        />
                    ))}
                </div>
            </CardContent>
        </Card>
    );
}
