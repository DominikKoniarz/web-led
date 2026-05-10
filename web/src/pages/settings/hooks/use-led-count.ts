import { env } from "@/env";
import { useMutation, useQueryClient } from "@tanstack/react-query";
import { useEffect, useState } from "react";

const useLEDCount = (ledCount: number) => {
    const [localLEDCount, setLocalLEDCount] = useState<string>(
        ledCount.toString(),
    );
    const isChanged = localLEDCount !== ledCount.toString();

    const queryClient = useQueryClient();
    useEffect(() => {
        setLocalLEDCount(ledCount.toString());
    }, [ledCount]);

    const ledMutation = useMutation({
        mutationFn: async (newLedCount: number) => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/settings`,
                {
                    method: "POST",
                    headers: {
                        Accept: "application/json",
                    },
                    body: JSON.stringify({
                        ledCount: newLedCount,
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
        onSuccess: () => {
            queryClient.invalidateQueries({ queryKey: ["settings"] });
        },
    });
    const onLEDCountChange = (ledCount: string) => {
        setLocalLEDCount(ledCount);
    };
    const saveLEDCount = () => {
        const parsedValue = parseInt(localLEDCount);
        if (isNaN(parsedValue)) {
            setLocalLEDCount("60");
            ledMutation.mutate(60);
        } else {
            ledMutation.mutate(parsedValue);
        }
    };
    return {
        localLEDCount,
        ledMutation,
        isChanged,
        onLEDCountChange,
        saveLEDCount,
    };
};

export default useLEDCount;
