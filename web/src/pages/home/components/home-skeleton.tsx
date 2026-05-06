import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
import { Skeleton } from "@/components/ui/skeleton";
import { Palette } from "lucide-react";

export function HomeSkeleton() {
    return (
        <div className="container mx-auto space-y-6 px-4 py-6">
            <Card>
                <CardHeader>
                    <CardTitle className="flex items-center gap-2">
                        <Palette className="text-muted-foreground h-5 w-5" />
                        <Skeleton className="h-5 w-24" />
                    </CardTitle>
                    <CardDescription>
                        <Skeleton className="h-4 w-56" />
                    </CardDescription>
                </CardHeader>

                <CardContent>
                    <div className="grid grid-cols-2 gap-3 sm:grid-cols-3 md:grid-cols-4">
                        {Array.from({ length: 8 }).map((_, index) => (
                            <div
                                key={index}
                                className="flex min-h-33 flex-col items-center justify-center rounded-xl border p-4"
                            >
                                <Skeleton className="mb-3 h-10 w-10 rounded-lg" />
                                <Skeleton className="mb-2 h-4 w-20" />
                                <Skeleton className="h-3 w-24" />
                            </div>
                        ))}
                    </div>
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle>
                        <Skeleton className="h-6 w-20" />
                    </CardTitle>
                    <CardDescription>
                        <Skeleton className="h-4 w-64" />
                    </CardDescription>
                </CardHeader>

                <CardContent className="space-y-6">
                    <div className="space-y-3">
                        <div className="flex items-center justify-between">
                            <Skeleton className="h-4 w-24" />
                            <Skeleton className="h-4 w-10" />
                        </div>

                        <div className="flex h-5 items-center">
                            <Skeleton className="h-4 w-full rounded-full" />
                        </div>
                    </div>

                    <div className="space-y-3">
                        <div className="flex items-center justify-between">
                            <Skeleton className="h-4 w-32" />
                            <Skeleton className="h-4 w-10" />
                        </div>

                        <div className="flex h-5 items-center">
                            <Skeleton className="h-4 w-full rounded-full" />
                        </div>
                    </div>
                </CardContent>
            </Card>
        </div>
    );
}
