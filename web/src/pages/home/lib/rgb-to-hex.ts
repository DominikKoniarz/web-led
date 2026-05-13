const toHex = (value: number) => {
    const hex = value.toString(16);
    return hex.length === 1 ? "0" + hex : hex;
};

export function rgbToHex(r: number, g: number, b: number) {
    return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
}
