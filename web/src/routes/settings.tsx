import { SettingsRouteComponent } from "@/pages/settings/settings";
import { createFileRoute } from "@tanstack/react-router";

export const Route = createFileRoute("/settings")({
    component: SettingsRouteComponent,
});
