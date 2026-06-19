import { createTheme } from "@mui/material/styles";

const theme = createTheme({
  palette: {
    mode: "dark",
    primary: {
      main: "#5B59D3",
      light: "#7875E8",
      dark: "#3D3BA8",
    },
    secondary: {
      main: "#3B9FFF",
      light: "#6BBFFF",
      dark: "#1E7FD6",
    },
    background: {
      default: "#111827",
      paper: "#1F2937",
    },
    success: { main: "#10B981" },
    error: { main: "#F87171" },
    warning: { main: "#FBBF24" },
    info: { main: "#60A5FA" },
    text: {
      primary: "#F3F4F6",
      secondary: "#D1D5DB",
      disabled: "#64748B",
    },
  },
  shape: {
    borderRadius: 12,
  },
  typography: {
    fontFamily: "Poppins, RenesasFont, cursive",
    fontSize: 13, // MUI base: ~13px for compact UI
    body1: { fontSize: "0.875rem" }, // 14px → keep readable
    body2: { fontSize: "0.8125rem" }, // 13px
    caption: { fontSize: "0.75rem" }, // 12px
    button: { fontSize: "0.8125rem", textTransform: "none" as const },
  },
});

export default theme;
