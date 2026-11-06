import { defineConfig } from "vite"
import react from "@vitejs/plugin-react-swc"
import path from "path"

// https://vite.dev/config/
export default defineConfig({
  // Overridden for the GitHub Pages build, which is served from a sub-path
  base: process.env.CONFIG_TOOL_BASE_PATH ?? "/",
  plugins: [react()],
  resolve: {
    alias: { "@src": path.resolve(__dirname, "./src"), }
  }
})
