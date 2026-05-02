import { Header } from "@/layouts/root/components/header";
import NavigationTabs from "@/layouts/root/components/navgiation-tabs";
import { Outlet } from "@tanstack/react-router";
import { TanStackRouterDevtools } from "@tanstack/react-router-devtools";

export function RootLayoutComponent() {
    return (
        <>
            <Header />
            <NavigationTabs />
            <Outlet />
            <TanStackRouterDevtools />
        </>
    );
}
