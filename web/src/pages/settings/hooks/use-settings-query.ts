import { env } from "@/env";
import type { SettingsState } from "@/pages/settings/types/settings-state";
import { useQuery } from "@tanstack/react-query";

const useSettingsQuery = () => {
    const {
        data: settingsState,
        error: settingsStateError,
        isLoading: isSettingsLoading,
    } = useQuery({
        queryKey: ["settings"],
        queryFn: async () => {
            const response = await fetch(
                `${env.VITE_API_BASE_URL}/api/settings`,
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
            return (await response.json()) as SettingsState;
        },
    });

    return {
        settingsState,
        settingsStateError,
        isSettingsLoading,
    };
};

export default useSettingsQuery;
