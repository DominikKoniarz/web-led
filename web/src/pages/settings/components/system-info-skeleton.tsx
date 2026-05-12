import { Skeleton } from "@/components/ui/skeleton";

export default function SystemInfoSkeleton() {
    return (
        <div className="rounded-xl border p-6">
            <div className="mb-6 space-y-2">
                <Skeleton className="h-6 w-24" />
                <Skeleton className="h-4 w-56" />
            </div>

            <div className="space-y-4">
                <div className="space-y-3 rounded-lg border p-4">
                    <div className="flex items-center justify-between">
                        <Skeleton className="h-4 w-28" />
                        <Skeleton className="h-4 w-16" />
                    </div>
                    <div className="flex items-center justify-between">
                        <Skeleton className="h-4 w-10" />
                        <Skeleton className="h-4 w-18" />
                    </div>
                    <div className="flex items-center justify-between">
                        <Skeleton className="h-4 w-24" />
                        <Skeleton className="h-4 w-16" />
                    </div>

                    <div className="flex items-center justify-between">
                        <Skeleton className="h-4 w-14" />
                        <Skeleton className="h-4 w-22" />
                    </div>

                    <div className="flex items-center justify-between">
                        <Skeleton className="h-4 w-20" />
                        <Skeleton className="h-4 w-14" />
                    </div>
                </div>

                <Skeleton className="h-10 w-full" />
            </div>
        </div>
    );
}
