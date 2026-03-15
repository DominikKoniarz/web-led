import tailwindcss from "@tailwindcss/vite";
import react from "@vitejs/plugin-react";
import { defineConfig } from "vite";
import compression from "vite-plugin-compression2";

// https://vite.dev/config/
export default defineConfig({
    plugins: [
        react(),
        tailwindcss(),
        compression({
            algorithms: ["gz"],
            deleteOriginalAssets: true,
            exclude: [/index\.html$/],
        }),
    ],
    build: {
        outDir: "../dist/web",
        emptyOutDir: true,
    },
});
