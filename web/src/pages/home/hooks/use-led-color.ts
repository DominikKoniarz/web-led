import { env } from "@/env";
import { hexToRgb } from "@/pages/home/lib/hex-to-rgb";
import { rgbToHex } from "@/pages/home/lib/rgb-to-hex";
import type { RGB } from "@/pages/home/types/rgb";
import { useMutation } from "@tanstack/react-query";
import { useEffect, useEffectEvent, useRef, useState } from "react";

const useLEDColor = (initialColor: { r: number; g: number; b: number }) => {
    const initialHex = rgbToHex(initialColor.r, initialColor.g, initialColor.b);
    const [color, setColor] = useState(initialHex);

    const colorInputRef = useRef<HTMLInputElement | null>(null);
    const rgb = hexToRgb(color);

    function handleColorChange(value: string) {
        setColor(value);
    }

    const timeoutRef = useRef<ReturnType<typeof setTimeout>>(null);
    const previousColor = useRef<string>(color);
    const colorMutation = useMutation({
        mutationFn: async (rgb: RGB) => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/leds/color`,
                {
                    method: "POST",
                    headers: {
                        Accept: "application/json",
                    },
                    body: JSON.stringify({
                        red: rgb.r,
                        green: rgb.g,
                        blue: rgb.b,
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
    const onColorChangeEvent = useEffectEvent((rgb: RGB) => {
        colorMutation.mutate(rgb);
    });
    useEffect(() => {
        if (timeoutRef.current) {
            clearTimeout(timeoutRef.current);
        }

        if (previousColor.current === color) {
            return;
        }

        timeoutRef.current = setTimeout(() => {
            if (rgb) {
                onColorChangeEvent(rgb);
            }
            previousColor.current = color;
        }, 200);

        return () => {
            if (timeoutRef.current) {
                clearTimeout(timeoutRef.current);
            }
        };
    }, [rgb, color]);
    return {
        color,
        rgb,
        handleColorChange,
        colorInputRef,
    };
};

export default useLEDColor;
