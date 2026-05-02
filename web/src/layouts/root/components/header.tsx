import { Radio } from "lucide-react";

export default function Header() {
    return (
        <header className="border-border bg-card border-b">
            <div className="container mx-auto px-4 py-4">
                <div className="flex items-center justify-between">
                    <div className="flex items-center gap-3">
                        <div className="bg-primary/10 flex h-10 w-10 items-center justify-center rounded-lg">
                            <Radio className="text-primary h-5 w-5" />
                        </div>
                        <div>
                            <h1 className="text-foreground text-lg font-semibold">
                                Web LED
                            </h1>
                            <p className="text-muted-foreground text-xs">
                                LED Strip Controller
                            </p>
                        </div>
                    </div>
                </div>
            </div>
        </header>
    );
}
