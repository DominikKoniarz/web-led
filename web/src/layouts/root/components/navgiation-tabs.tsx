import { Tabs, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { useLocation, useRouter } from "@tanstack/react-router";
import { Lightbulb, Settings } from "lucide-react";

export default function NavigationTabs() {
    const location = useLocation();
    const router = useRouter();

    const handleTabChange = (value: string) => {
        router.navigate({
            to: value,
        });
    };

    return (
        <main className="container mx-auto px-4 py-6">
            <Tabs
                value={location.pathname}
                onValueChange={handleTabChange}
                className="space-y-6"
            >
                <TabsList className="grid w-full max-w-md grid-cols-2">
                    <TabsTrigger value="/" className="flex items-center gap-2">
                        <Lightbulb className="h-4 w-4" />
                        LED Modes
                    </TabsTrigger>
                    <TabsTrigger
                        value="/settings"
                        className="flex items-center gap-2"
                    >
                        <Settings className="h-4 w-4" />
                        Settings
                    </TabsTrigger>
                </TabsList>
            </Tabs>
        </main>
    );
}
