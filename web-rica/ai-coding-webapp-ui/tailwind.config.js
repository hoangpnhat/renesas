/** @type {import('tailwindcss').Config} */
export default {
  content: ["./index.html", "./src/**/*.{js,ts,jsx,tsx}"],
  darkMode: "class",
  theme: {
    extend: {
      animation: {
        typewriter: "typewriter 2s steps(11) forwards",
        "width-bounce": "width-bounce 1s ease-in-out",
        fadeIn: "fadeIn 0.5s ease-out forwards",
        caret:
          "typewriter 2s steps(11) forwards, blink 1s steps(11) infinite 2s",
        "focus-flash": "focus-flash 4s ease-out forwards",
      },
      keyframes: {
        "width-bounce": {
          "0%": { width: "2rem" },
          "50%": { width: "12rem" },
          "100%": { width: "6rem" },
        },
        fadeIn: {
          "0%": { opacity: "0", transform: "translateY(20px)" },
          "100%": { opacity: "1", transform: "translateY(0)" },
        },
        typewriter: {
          to: {
            left: "100%",
          },
        },
        blink: {
          "0%": { opacity: "0" },
          "0.1%": { opacity: "1" },
          "50%": { opacity: "1" },
          "50.1%": { opacity: "0" },
          "100%": { opacity: "0" },
        },
        "focus-flash": {
          "0%": { boxShadow: "0 0 0 3px rgba(91,89,211,0.6)" },
          "100%": { boxShadow: "0 0 0 0px rgba(91,89,211,0)" },
        },
      },
      colors: {
        // Primary palette
        primary: {
          main: "#5B59D3",
          light: "#7875E8",
          dark: "#3D3BA8",
        },
        // Secondary palette
        secondary: {
          main: "#3B9FFF",
          light: "#6BBFFF",
          dark: "#1E7FD6",
        },
        // Background palette
        background: {
          default: "#111827",
          paper: "#1F2937",
          elevated: "#1E293B",
          darker: "#0F172A",
        },
        // Surface palette
        surface: {
          base: "#1E293B",
          light: "#334155",
          lighter: "#475569",
          lightest: "#64748B",
        },
        // Text palette
        text: {
          primary: "#F3F4F6",
          secondary: "#D1D5DB",
          tertiary: "#CBD5E1",
          muted: "#94A3B8",
          disabled: "#64748B",
        },
        // Semantic palette
        success: { main: "#10B981" },
        error: { main: "#F87171" },
        warning: { main: "#FBBF24" },
        info: { main: "#60A5FA" },
        // Border palette
        border: {
          default: "#334155",
          light: "#475569",
          lighter: "#64748B",
        },
        // Specialty colors
        content: {
          user: "#E0E0E0", // user chat bubble background
          bot: "#F7F7F7", // bot chat bubble background
        },
        accent: "#2A2A2A", // dark text on light chat bubbles
      },
    },
  },
  plugins: [],
};
