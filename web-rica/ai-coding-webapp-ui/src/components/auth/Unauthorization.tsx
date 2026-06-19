/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */


import { useEffect } from "react";
import { useMsal } from "@azure/msal-react";
import { loginRequest } from "../../msal/authConfig.ts";

export default function SignInSide() {
  const { instance } = useMsal();

  // Xử lý login redirect sau khi quay lại từ Azure AD
  useEffect(() => {
    instance
      .handleRedirectPromise()
      .then((response) => {
        if (response && response.account) {
          instance.setActiveAccount(response.account);
        } else {
          const currentAccounts = instance.getAllAccounts();
          if (currentAccounts.length > 0) {
            instance.setActiveAccount(currentAccounts[0]);
          }
        }
      })
      .catch((error) => {
        console.error("Redirect login error:", error);
      });
  }, [instance]);

  const handleLogin = async () => {
    try {
      await instance.loginRedirect(loginRequest);
    } catch (error) {
      console.error("Login error:", error);
    }
  };

  const handleLogout = async () => {
    try {
      await instance.logoutRedirect({
        account: instance.getActiveAccount() ?? undefined,
      });
    } catch (error) {
      console.error("Logout error:", error);
    }
  };

  const activeAccount = instance.getActiveAccount();

  return (
    <div
      style={{
        minHeight: "100vh",
        display: "flex",
        justifyContent: "center",
        alignItems: "center",
        background: "#f5f5f5",
      }}
    >
      <div
        style={{
          padding: "2rem",
          borderRadius: "12px",
          background: "#fff",
          boxShadow: "0 4px 12px rgba(0,0,0,0.1)",
          textAlign: "center",
          minWidth: "320px",
        }}
      >
        {activeAccount ? (
          <>
            <h2>Welcome, {activeAccount.username}</h2>
            <p>You are logged in.</p>
            <button
              onClick={handleLogout}
              style={{
                marginTop: "1rem",
                padding: "0.75rem 1.5rem",
                border: "none",
                borderRadius: "8px",
                background: "#d32f2f",
                color: "#fff",
                fontWeight: "bold",
                cursor: "pointer",
              }}
            >
              Sign Out
            </button>
          </>
        ) : (
          <>
            <h2>Please sign in</h2>
            <button
              onClick={handleLogin}
              style={{
                marginTop: "1rem",
                padding: "0.75rem 1.5rem",
                border: "none",
                borderRadius: "8px",
                background: "#0078d4",
                color: "#fff",
                fontWeight: "bold",
                cursor: "pointer",
              }}
            >
              Sign In
            </button>
          </>
        )}
      </div>
    </div>
  );
}
