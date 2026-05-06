import { env } from "@/env";
import { useMutation } from "@tanstack/react-query";
import { useEffect, useEffectEvent, useRef, useState } from "react";

const useSpeed = (initialSpeedPercent: number) => {
    const [speedPercent, setSpeedPercent] = useState(initialSpeedPercent);

    const timeoutRef = useRef<ReturnType<typeof setTimeout>>(null);
    const previousSpeed = useRef<number>(initialSpeedPercent);

    const speedMutation = useMutation({
        mutationFn: async (newSpeed: number) => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/leds/speed`,
                {
                    method: "POST",
                    headers: {
                        Accept: "application/json",
                    },
                    body: JSON.stringify({
                        speedPercent: newSpeed,
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

    const onSpeedValueChange = (speedPercent: number) => {
        setSpeedPercent(speedPercent);
    };

    const onSpeedChangeEffect = useEffectEvent((speed: number) => {
        speedMutation.mutate(speed);
    });

    useEffect(() => {
        if (timeoutRef.current) {
            clearTimeout(timeoutRef.current);
        }

        if (previousSpeed.current === speedPercent) {
            return;
        }

        timeoutRef.current = setTimeout(() => {
            onSpeedChangeEffect(speedPercent);

            previousSpeed.current = speedPercent;
        }, 200);
    }, [speedPercent]);

    return {
        onSpeedValueChange,
        speedPercent,
        setSpeedPercent,
    };
};

export default useSpeed;
