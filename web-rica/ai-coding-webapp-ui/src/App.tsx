/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { RouterProvider } from "react-router-dom";
import { router } from "./routes";
import "./App.scss";
import { Fragment } from "react";
import { ToastContainer } from "react-toastify";
import "react-toastify/dist/ReactToastify.css";

import { AuthenticatedTemplate, UnauthenticatedTemplate } from "@azure/msal-react";

import SignInSide from "./components/auth/Unauthorization.tsx";

function App() {
  return (
    <Fragment>
      <AuthenticatedTemplate>
        <RouterProvider router={router} />
      </AuthenticatedTemplate>
      <UnauthenticatedTemplate>
        <SignInSide />
      </UnauthenticatedTemplate>
      <ToastContainer
        position="top-right"
        autoClose={3000}
        hideProgressBar={false}
        newestOnTop
        closeOnClick
        pauseOnHover
      />
    </Fragment>
  );
}

export default App;
