import { Skeleton } from "@/components/ui/skeleton";

export default function SettingsSkeleton() {
    return (
        <div>
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
                <div className="flex justify-end">
                    <Skeleton className="h-9 w-32" />
                </div>
            </div>
        </div>
    );
}
