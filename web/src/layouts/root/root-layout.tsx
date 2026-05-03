import Header from "@/layouts/root/components/header";
import NavigationTabs from "@/layouts/root/components/navgiation-tabs";
import QueryClientProvider from "@/providers/query-client-provider";
import { Outlet } from "@tanstack/react-router";
import { TanStackRouterDevtools } from "@tanstack/react-router-devtools";

export function RootLayoutComponent() {
    return (
        <QueryClientProvider>
            <Header />
            <NavigationTabs />
            <Outlet />
            <TanStackRouterDevtools />
        </QueryClientProvider>
    );
}
