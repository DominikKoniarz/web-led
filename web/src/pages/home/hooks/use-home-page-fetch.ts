import { env } from "@/env";
import type { LEDState } from "@/pages/home/types/led";
import { useQuery } from "@tanstack/react-query";

export const useHomePageFetch = () => {
    const {
        data: ledState,
        error: ledStateError,
        isLoading: isLedStateLoading,
    } = useQuery({
        queryKey: ["leds"],
        queryFn: async () => {
            const response = await fetch(`${env.VITE_API_BASE_URL}/api/leds`, {
                headers: {
                    Accept: "application/json",
                },
            });

            if (!response.ok) {
                const message = await response.text();
                throw new Error(
                    `Request failed (${response.status}): ${message}`,
                );
            }

            return (await response.json()) as LEDState;
        },
    });

    return {
        ledState,
        ledStateError,
        isLedStateLoading,
    };
};
