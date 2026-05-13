import useLEDColor from "@/pages/home/hooks/use-led-color";

const colorPresets = [
    "#ef4444",
    "#f97316",
    "#eab308",
    "#22c55e",
    "#06b6d4",
    "#3b82f6",
    "#8b5cf6",
    "#ec4899",
    "#ffffff",
];
type Props = {
    color: { r: number; g: number; b: number };
};
export default function LEDColor({ color: initialColor }: Props) {
    const { color, rgb, handleColorChange, colorInputRef } =
        useLEDColor(initialColor);
    return (
        <div className="rounded-2xl border border-neutral-800 bg-neutral-950 p-8">
            <div className="mb-8">
                <h2 className="text-xl font-semibold text-white">
                    Solid Color Settings
                </h2>
                <p className="mt-2 text-sm text-neutral-400">
                    Configure selected LED effect
                </p>
            </div>

            <div className="space-y-8">
                <div>
                    <label className="mb-4 block text-sm font-medium text-white">
                        Color
                    </label>

                    <div className="flex flex-wrap items-center gap-3">
                        {colorPresets.map((preset) => {
                            const isSelected =
                                preset.toLowerCase() === color.toLowerCase();

                            return (
                                <button
                                    key={preset}
                                    type="button"
                                    onClick={() => handleColorChange(preset)}
                                    className="relative h-9 w-9 rounded-full transition hover:scale-110"
                                    style={{ backgroundColor: preset }}
                                    aria-label={`Select color ${preset}`}
                                >
                                    {isSelected && (
                                        <span className="absolute inset-[-4px] rounded-full border-2 border-emerald-400" />
                                    )}
                                </button>
                            );
                        })}

                        <button
                            type="button"
                            onClick={() => colorInputRef.current?.click()}
                            className="flex h-9 w-9 items-center justify-center rounded-full border border-neutral-700 bg-neutral-900 text-lg text-neutral-300 transition hover:border-emerald-400 hover:text-emerald-400"
                            aria-label="Pick custom color"
                        >
                            +
                        </button>

                        <input
                            ref={colorInputRef}
                            type="color"
                            value={color}
                            onChange={(event) =>
                                handleColorChange(event.target.value)
                            }
                            className="sr-only"
                        />
                        <div className="w-full">
                            <label className="mb-4 block text-sm font-medium text-white">
                                Selected
                            </label>

                            <div className="grid gap-5">
                                <button
                                    type="button"
                                    onClick={() =>
                                        colorInputRef.current?.click()
                                    }
                                    className="h-32 rounded-xl border border-neutral-800 transition hover:border-emerald-400"
                                    style={{
                                        backgroundColor: color,
                                        boxShadow: `0 0 48px ${color}33`,
                                    }}
                                    aria-label="Open color picker"
                                />
                            </div>
                        </div>
                    </div>
                </div>

                <div className="flex flex-col justify-center gap-4">
                    {rgb && (
                        <div className="flex gap-2">
                            <h3 className="text-white">RGB</h3>
                            <span className="text-neutral-400">
                                {rgb.r}, {rgb.g}, {rgb.b}
                            </span>
                        </div>
                    )}

                    <div className="flex gap-2">
                        <h3 className="text-white">HEX</h3>
                        <span className="text-neutral-400">{color}</span>
                    </div>
                </div>
            </div>
        </div>
    );
}
