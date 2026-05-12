import { env } from "@/env";
import type { WifiStatus } from "@/pages/settings/types/wifi";
import { useQuery } from "@tanstack/react-query";

const useNetworkStatus = () => {
    const {
        data: wifiStatus,
        error,
        isLoading,
    } = useQuery({
        queryKey: ["wifi-status"],
        queryFn: async () => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/wifi/status`,
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
            return (await response.json()) as WifiStatus;
        },
    });
    return {
        wifiStatus,
        error,
        isLoading,
    };
};
export default useNetworkStatus;
