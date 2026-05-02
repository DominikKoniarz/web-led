import { RootLayoutComponent } from "@/pages/root-layout";
import { createRootRoute } from "@tanstack/react-router";

export const Route = createRootRoute({
    component: RootLayoutComponent,
});
