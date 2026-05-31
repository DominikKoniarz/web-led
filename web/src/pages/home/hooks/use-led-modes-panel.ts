import { env } from "@/env";
import type { LEDMode } from "@/pages/home/types/led";
import { useMutation, useQueryClient } from "@tanstack/react-query";
import { useEffect, useEffectEvent, useRef, useState } from "react";

const useLEDModesPanel = (mode: LEDMode) => {
    const [selectedMode, setSelectedMode] = useState<LEDMode>(mode);

    const timeoutRef = useRef<ReturnType<typeof setTimeout>>(null);
    const previousMode = useRef<LEDMode>(selectedMode);
    const queryClient = useQueryClient();
    const ledModeMutation = useMutation({
        mutationFn: async (newMode: LEDMode) => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/leds/mode`,
                {
                    method: "POST",
                    headers: {
                        Accept: "application/json",
                    },
                    body: JSON.stringify({
                        mode: newMode,
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
        // onMutate: async () => {
        //     const previousMode = selectedMode;

        //     return { previousMode };
        // },
        onSuccess: () => {
            queryClient.invalidateQueries({ queryKey: ["leds"] });
        },
        onError: () => {
            // TODO: add toast
        },
    });

    const onSelect = (mode: LEDMode) => {
        setSelectedMode(mode);
    };

    const onSelectedModeChangeEffect = useEffectEvent((mode: LEDMode) => {
        ledModeMutation.mutate(mode);
    });

    useEffect(() => {
        if (timeoutRef.current) {
            clearTimeout(timeoutRef.current);
        }

        if (previousMode.current === selectedMode) {
            return;
        }

        timeoutRef.current = setTimeout(() => {
            onSelectedModeChangeEffect(selectedMode);

            previousMode.current = selectedMode;
        }, 200);
    }, [selectedMode]);

    return {
        onSelect,
        selectedMode,
    };
};

export default useLEDModesPanel;
