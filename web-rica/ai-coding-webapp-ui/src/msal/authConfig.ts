/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

/**
 * Configuration object to be passed to MSAL instance on creation.
 */
export const loginRequest = {
  scopes: [
    import.meta.env.VITE_AZURE_ENTRA_SCOPES ||
      "api://30a4cd4a-6cd0-42d7-86aa-45e7cefc5b3f/user_impersonation",
  ],
};
export const msalConfig = {
  auth: {
    clientId:
      import.meta.env.VITE_AZURE_ENTRA_CLIENT_ID ||
      "a0af2b5f-e4fb-4c47-bb35-43f64afd5631",
    usePkceWithAuthorizationCodeGrant: true,
    authority:
      import.meta.env.VITE_AZURE_ENTRA_AUTHORITY ||
      "https://login.microsoftonline.com/53d82571-da19-47e4-9cb4-625a166a4a2a/v2.0",
    redirectUri: import.meta.env.VITE_AZURE_ENTRA_REDIRECT_UI, //eg: ${window.location.origin}/Dashboard
    postLogoutRedirectUri: "/",
    navigateToLoginRequestUrl: false,
  },
  cache: {
    cacheLocation: "sessionStorage", // "sessionStorage" or"localStorage"
    storeAuthStateInCookie: false,
  },
  system: {
    allowNativeBroker: false,
  },
};

/**
 * Scopes you add here will be prompted for user consent during sign-in.
 * By default, MSAL.js will add OIDC scopes (openid, profile, email) to any login request.
 //  */
// export const loginRequest = {
//     scopes: [
//         "User.Read",
//         "api://30a4cd4a-6cd0-42d7-86aa-45e7cefc5b3f/user_impersonation",
//     ],
// };

/**
 * An optional silentRequest object can be used to achieve silent SSO
 * between applications by providing a "login_hint" property.
 */
export const silentRequest = {
  scopes: ["openid", "profile"],
  loginHint: "example@domain.net",
};

/**
 * Add here the scopes to request when obtaining an access token for MS Graph API. For more information, see:
 * https://github.com/AzureAD/microsoft-authentication-library-for-js/blob/dev/lib/msal-browser/docs/resources-and-scopes.md
 */
export const graphConfig = {
  graphMeEndpoint: "https://graph.microsoft.com/v1.0/me", //e.g. https://graph.microsoft.com/v1.0/me
  graphMePhotEndpoint:
    "https://graph.microsoft.com/v1.0/me/photos/64x64/$value",
};
