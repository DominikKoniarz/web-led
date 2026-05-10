import { env } from "@/env";
import type { WifiScanResponse } from "@/pages/settings/types/wifi";
import { useQuery } from "@tanstack/react-query";
import { useEffect, useEffectEvent } from "react";

const useWifiScan = () => {
    const { data, error, isFetching, refetch } = useQuery<WifiScanResponse>({
        queryKey: ["wifi-scan"],
        enabled: false,
        queryFn: async () => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/wifi/scan`,
                {
                    headers: {
                        Accept: "application/json",
                    },
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
        refetchOnWindowFocus: false,
    });
    const onDataChangeEffect = useEffectEvent(() => {
        refetch();
    });

    useEffect(() => {
        let timer: ReturnType<typeof setTimeout> | null = null;

        if (data?.status === "started" || data?.status === "running") {
            timer = setTimeout(() => {
                onDataChangeEffect();
            }, 2000);
        }

        return () => {
            if (timer) {
                clearTimeout(timer);
            }
        };
    }, [data]);

    const isScanning =
        isFetching || data?.status === "started" || data?.status === "running";

    return {
        data,
        error,
        isScanning,
        refetch,
    };
};

export default useWifiScan;
