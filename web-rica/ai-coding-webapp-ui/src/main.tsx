/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import ReactDOM from "react-dom/client";
import App from "./App.tsx";
import "./index.scss";

import CssBaseline from "@mui/material/CssBaseline";
import ThemeProvider from "@mui/material/styles/ThemeProvider";
import theme from "./layout/CustomTheme.tsx";
import { MsalProvider } from "@azure/msal-react";
import { Provider } from "react-redux";
import { store } from "./store";
import { msalInstance } from "./msal/auth.tsx";
import "./i18n/i18n.ts";

msalInstance.initialize();

ReactDOM.createRoot(document.getElementById("root")!).render(
  <Provider store={store}>
    <MsalProvider instance={msalInstance}>
      <ThemeProvider theme={theme}>
        <CssBaseline />
        <App />
      </ThemeProvider>
    </MsalProvider>
  </Provider>,
);
