import { Tabs, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Lightbulb, Settings } from "lucide-react";
export default function NavigationTabs() {
    return (
        <main className="container mx-auto px-4 py-6">
            <Tabs
                // value={activeTab}
                // onValueChange={setActiveTab}
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
