import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Settings } from "lucide-react";

export default function DeviceSettings() {
    return (
        <Card>
            <CardHeader>
                <CardTitle className="flex items-center gap-2">
                    <Settings className="text-primary h-5 w-5" />
                    Device Settings
                </CardTitle>
                <CardDescription>Configure device LED count</CardDescription>
            </CardHeader>
            <CardContent className="space-y-6">
                <div className="space-y-2">
                    <Label htmlFor="led-count">LED Count</Label>
                    <Input
                        id="led-count"
                        type="number"
                        defaultValue="60"
                        placeholder="Number of LEDs"
                    />
                    <p className="text-muted-foreground text-xs">
                        Total number of LEDs in your strip
                    </p>
                </div>
            </CardContent>
        </Card>
    );
}
