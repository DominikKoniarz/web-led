import { Button } from "@/components/ui/button";
import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { Info, RotateCcw } from "lucide-react";

export default function SystemInfo() {
    return (
        <Card>
            <CardHeader>
                <CardTitle className="flex items-center gap-2">
                    <Info className="text-primary h-5 w-5" />
                    System
                </CardTitle>
                <CardDescription>
                    System actions and information
                </CardDescription>
            </CardHeader>
            <CardContent className="space-y-4">
                <div className="bg-secondary/50 grid gap-2 rounded-lg p-4 text-sm">
                    <div className="flex items-center justify-between">
                        <span className="text-muted-foreground">
                            Firmware Version
                        </span>
                        <span className="text-foreground font-mono">
                            v1.0.0
                        </span>
                    </div>
                    <div className="flex items-center justify-between">
                        <span className="text-muted-foreground">Chip</span>
                        <span className="text-foreground font-mono">
                            ESP32-WROOM
                        </span>
                    </div>
                    <div className="flex items-center justify-between">
                        <span className="text-muted-foreground">
                            Free Memory
                        </span>
                        <span className="text-foreground font-mono">
                            245 KB
                        </span>
                    </div>
                </div>

                <Button variant="outline" className="w-full cursor-pointer">
                    <RotateCcw className="mr-2 h-4 w-4" />
                    Restart Device
                </Button>
            </CardContent>
        </Card>
    );
}
