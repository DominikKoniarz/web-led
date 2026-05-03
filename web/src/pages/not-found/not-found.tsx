import { useRouter } from "@tanstack/react-router";
import { useLayoutEffect } from "react";

export default function NotFoundComponent() {
    const router = useRouter();

    useLayoutEffect(() => {
        router.navigate({
            to: "/",
        });
    }, [router]);

    return <main className="bg-background" />;
}
