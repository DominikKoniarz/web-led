import { HomeRouteComponent } from "@/pages/home/home";
import { createFileRoute } from "@tanstack/react-router";

export const Route = createFileRoute("/")({
    component: HomeRouteComponent,
});
