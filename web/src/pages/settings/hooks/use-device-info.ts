import { env } from "@/env";
import type { DeviceInfo } from "@/pages/settings/types/device-info";
import { useQuery } from "@tanstack/react-query";

const useDeviceInfo = () => {
    const { data, error, isLoading } = useQuery({
        queryKey: ["system-info"],
        queryFn: async () => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/system/health`,
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
            return (await response.json()) as DeviceInfo;
        },
        refetchInterval: 5000,
    });
    return {
        data,
        error,
        isLoading,
    };
};

export default useDeviceInfo;
