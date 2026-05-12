import NetworkItem from "@/pages/settings/components/network-item";
import type { Network } from "@/pages/settings/types/wifi";

type Props = {
    networks: Network[];
};

export default function AvailableNetworksList({ networks }: Props) {
    return (
        <div className="space-y-2">
            {networks.map((network, index) => (
                <NetworkItem
                    key={`${network.ssid}-${index}`}
                    network={network}
                />
            ))}
        </div>
    );
}
