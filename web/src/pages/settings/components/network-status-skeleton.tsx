import { Card, CardContent, CardHeader } from "@/components/ui/card";
import { Skeleton } from "@/components/ui/skeleton";

export default function NetworkStatusSkeleton() {
    return (
        <Card>
            <CardHeader>
                <Skeleton className="h-6 w-32" />
            </CardHeader>
            <CardContent className="space-y-4">
                <div className="space-y-4">
                    <div className="bg-secondary/50 grid gap-2 rounded-lg p-4">
                        <Skeleton className="h-6 w-38" />
                        <div className="flex items-center justify-between">
                            <Skeleton className="h-4 w-24" />
                            <Skeleton className="h-4 w-26" />
                        </div>
                        <div className="flex items-center justify-between">
                            <Skeleton className="h-4 w-18" />
                            <Skeleton className="h-4 w-18" />
                        </div>
                    </div>
                </div>

                <div className="space-y-4">
                    <div className="bg-secondary/50 flex flex-col gap-2 rounded-lg p-4">
                        <Skeleton className="h-6 w-26" />
                        <div className="flex items-center justify-between">
                            <Skeleton className="h-4 w-24" />
                            <Skeleton className="h-4 w-32" />
                        </div>
                        <div className="flex items-center justify-between">
                            <Skeleton className="h-4 w-18" />
                            <Skeleton className="h-4 w-28" />
                        </div>
                    </div>
                </div>
            </CardContent>
        </Card>
    );
}
