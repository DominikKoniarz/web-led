export function hexToRgb(hex: string) {
    const cleanHex = hex.replace("#", "");

    if (cleanHex.length !== 6) {
        return null;
    }

    const r = parseInt(cleanHex.slice(0, 2), 16);
    const g = parseInt(cleanHex.slice(2, 4), 16);
    const b = parseInt(cleanHex.slice(4, 6), 16);

    return { r, g, b };
}
