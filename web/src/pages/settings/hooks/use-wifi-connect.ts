import { env } from "@/env";
import { useMutation } from "@tanstack/react-query";

const useWifiConnect = () => {
    const wifiConnect = useMutation({
        mutationFn: async ({
            ssid,
            password,
        }: {
            ssid: string;
            password: string;
        }) => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/wifi/connect`,
                {
                    method: "POST",
                    headers: {
                        Accept: "application/json",
                    },
                    body: JSON.stringify({
                        ssid,
                        password,
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
    return {
        wifiConnect,
    };
};

export default useWifiConnect;
