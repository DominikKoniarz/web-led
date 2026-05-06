import { Skeleton } from "@/components/ui/skeleton";

export default function SettingsSkeleton() {
    return (
        <div className="container mx-auto space-y-6 px-4 py-6">
            <div className="rounded-xl border p-6">
                <div className="mb-6 flex items-center justify-between">
                    <div className="space-y-2">
                        <Skeleton className="h-6 w-44" />
                        <Skeleton className="h-4 w-56" />
                    </div>
                    <Skeleton className="h-9 w-24" />
                </div>

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
            </div>

            <div className="rounded-xl border p-6">
                <div className="mb-6 space-y-2">
                    <Skeleton className="h-6 w-40" />
                    <Skeleton className="h-4 w-52" />
                </div>

                <div className="space-y-2">
                    <Skeleton className="h-4 w-20" />
                    <Skeleton className="h-10 w-full" />
                    <Skeleton className="h-3 w-48" />
                </div>
            </div>

            <div className="rounded-xl border p-6">
                <div className="mb-6 space-y-2">
                    <Skeleton className="h-6 w-24" />
                    <Skeleton className="h-4 w-56" />
                </div>

                <div className="space-y-4">
                    <div className="space-y-3 rounded-lg border p-4">
                        <div className="flex items-center justify-between">
                            <Skeleton className="h-4 w-28" />
                            <Skeleton className="h-4 w-20" />
                        </div>
                        <div className="flex items-center justify-between">
                            <Skeleton className="h-4 w-16" />
                            <Skeleton className="h-4 w-28" />
                        </div>
                        <div className="flex items-center justify-between">
                            <Skeleton className="h-4 w-24" />
                            <Skeleton className="h-4 w-16" />
                        </div>
                    </div>

                    <Skeleton className="h-10 w-full" />
                </div>
            </div>
        </div>
    );
}
