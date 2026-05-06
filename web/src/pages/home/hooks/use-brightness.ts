import { env } from "@/env";
import { useMutation } from "@tanstack/react-query";
import { useEffect, useEffectEvent, useRef, useState } from "react";

const useBrightness = (initialBrightnessPercent: number) => {
    const [brightnessPercent, setBrightnessPercent] = useState(
        initialBrightnessPercent,
    );

    const timeoutRef = useRef<ReturnType<typeof setTimeout>>(null);
    const previousBrightness = useRef<number>(initialBrightnessPercent);

    const brightnessMutation = useMutation({
        mutationFn: async (newBrightness: number) => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/leds/brightness`,
                {
                    method: "POST",
                    headers: {
                        Accept: "application/json",
                    },
                    body: JSON.stringify({
                        brightnessPercent: newBrightness,
                    }),
                },
            );
            if (!response.ok) {
                const message = await response.text();
                throw new Error(
                    `Request failed (${response.status}): ${message}`,
                );
            }

            return response.json();
        },
    });
    const onBrightnessValueChange = (brightnessPercent: number) => {
        setBrightnessPercent(brightnessPercent);
    };

    const onBrightnessChangeEffect = useEffectEvent((brightness: number) => {
        brightnessMutation.mutate(brightness);
    });

    useEffect(() => {
        if (timeoutRef.current) {
            clearTimeout(timeoutRef.current);
        }

        if (previousBrightness.current === brightnessPercent) {
            return;
        }

        timeoutRef.current = setTimeout(() => {
            onBrightnessChangeEffect(brightnessPercent);

            previousBrightness.current = brightnessPercent;
        }, 200);
    }, [brightnessPercent]);

    return {
        onBrightnessValueChange,
        brightnessPercent,
        setBrightnessPercent,
    };
};

export default useBrightness;
