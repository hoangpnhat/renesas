import { defineConfig, PluginOption } from "vite";
import react from "@vitejs/plugin-react-swc";
import { visualizer } from "rollup-plugin-visualizer";
import viteImagemin from "vite-plugin-imagemin";

// https://vitejs.dev/config/
export default defineConfig(({ mode }) => {
  const isAnalyze = mode === "analyze";
  return {
    // base: "/agnes",
    build: {
      emptyOutDir: true,
      // sourcemap: true,
      rollupOptions: {
        output: {
          manualChunks: {
            react: ["react", "react-router-dom", "react-dom"],
            markdown: ["react-markdown"],
            react_highlight: ["react-syntax-highlighter"],
            azure: ["@azure/msal-browser", "@azure/msal-react"],
            mui: ["@mui/material"],
            muiIcon: ["@mui/icons-material"],
            axios: ["axios"],
            redux: ["react-redux", "@reduxjs/toolkit"],
            dayjs: ["dayjs"],
            i18n: ["i18next", "react-i18next"],
            yup_form: ["yup", "react-hook-form"],
            x_date_picker: ["@mui/x-date-pickers"],
          },
        },
      },
    },

    plugins: [
      react(),
      viteImagemin({
        gifsicle: {
          optimizationLevel: 7,
          interlaced: false,
        },
        optipng: {
          optimizationLevel: 7,
        },
        mozjpeg: {
          quality: 20,
        },
        pngquant: {
          quality: [0.8, 0.9],
          speed: 4,
        },
        svgo: {
          plugins: [
            {
              name: "removeViewBox",
            },
            {
              name: "removeEmptyAttrs",
              active: false,
            },
          ],
        },
      }),
      visualizer({
        gzipSize: true,
        brotliSize: true,
        emitFile: false,
        filename: "stat.html",
        open: isAnalyze,
      }) as PluginOption,
    ],
  };
});
