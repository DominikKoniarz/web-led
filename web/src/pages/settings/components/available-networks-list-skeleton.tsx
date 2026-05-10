import { Skeleton } from "@/components/ui/skeleton";

export default function AvailableNetworksListSkeleton() {
    return (
        <div className="space-y-2">
            {Array.from({ length: 4 }).map((_, index) => (
                <div
                    key={index}
                    className="flex items-center justify-between rounded-lg border p-3"
                >
                    <div className="flex items-center gap-3">
                        <Skeleton className="h-4 w-4 rounded-full" />
                        <Skeleton className="h-5 w-36" />
                    </div>
                    <Skeleton className="h-4 w-4" />
                </div>
            ))}
        </div>
    );
}
