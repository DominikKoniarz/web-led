import { cn } from "@/lib/utils";
import {
    Check,
    Circle,
    Flame,
    Power,
    Rainbow,
    Sparkles,
    Waves,
    Wind,
    Zap,
} from "lucide-react";
import type { CSSProperties } from "react";

interface LEDMode {
    id: string;
    name: string;
    description: string;
    icon: string;
    color?: string;
}

const iconMap: Record<
    string,
    React.ComponentType<{ className?: string; style: CSSProperties }>
> = {
    circle: Circle,
    rainbow: Rainbow,
    wind: Wind,
    zap: Zap,
    sparkles: Sparkles,
    flame: Flame,
    waves: Waves,
    power: Power,
};

interface LEDModeCardProps {
    mode: LEDMode;
    isSelected: boolean;
    onSelect: () => void;
}

export function LEDModeCard({ mode, isSelected, onSelect }: LEDModeCardProps) {
    const Icon = iconMap[mode.icon] || Circle;

    return (
        <button
            onClick={onSelect}
            className={cn(
                "group relative flex cursor-pointer flex-col items-center gap-3 rounded-lg border p-4 text-left transition-all",
                "hover:border-primary/50 hover:bg-secondary/50",
                isSelected
                    ? "border-primary bg-primary/10"
                    : "border-border bg-card",
            )}
        >
            {isSelected && (
                <div className="absolute top-2 right-2">
                    <div className="bg-primary flex h-5 w-5 items-center justify-center rounded-full">
                        <Check className="text-primary-foreground h-3 w-3" />
                    </div>
                </div>
            )}

            <div
                className={cn(
                    "flex h-12 w-12 items-center justify-center rounded-lg transition-colors",
                    isSelected ? "bg-primary/20" : "bg-secondary",
                )}
                style={{
                    backgroundColor: isSelected ? `${mode.color}20` : undefined,
                }}
            >
                <Icon
                    className="h-6 w-6 transition-colors"
                    style={{ color: mode.color }}
                />
            </div>

            <div className="text-center">
                <h3 className="text-foreground font-medium">{mode.name}</h3>
                <p className="text-muted-foreground mt-0.5 text-xs">
                    {mode.description}
                </p>
            </div>
        </button>
    );
}
