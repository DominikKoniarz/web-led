import { RootLayoutComponent } from "@/layouts/root/root-layout";
import NotFoundComponent from "@/pages/not-found/not-found";
import { createRootRoute } from "@tanstack/react-router";

export const Route = createRootRoute({
    component: RootLayoutComponent,
    notFoundComponent: NotFoundComponent,
});
