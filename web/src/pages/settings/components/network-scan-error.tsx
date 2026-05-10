import { Button } from "@/components/ui/button";
import {
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
} from "@/components/ui/card";
type Props = {
    refetch: () => void;
};
export default function NetworkScanError({ refetch }: Props) {
    return (
        <Card>
            <CardHeader>
                <CardTitle>Available Networks</CardTitle>
                <CardDescription>
                    Failed to scan available networks
                </CardDescription>
            </CardHeader>
            <CardContent>
                <Button onClick={() => refetch()}>Try again</Button>
            </CardContent>
        </Card>
    );
}
