import { PublicClientApplication } from "@azure/msal-browser";
import { loginRequest, msalConfig } from "./authConfig";
import { store } from "../store";
import { updateUserDetails } from "../store/reducers/user.slice";

export const msalInstance = new PublicClientApplication(msalConfig);

export const acquireToken = async (scopes?: string[]) => {
  const account = msalInstance.getAllAccounts()[0];
  let scopes_ = [...loginRequest.scopes];
  if (account) {
    try {
      if (scopes !== undefined) {
        scopes_ = [...scopes];
      }
      const response = await msalInstance.acquireTokenSilent({
        ...loginRequest,
        scopes: scopes_,
        account: account
      });
      if (store.getState().user.roles.length === 0) {
        store.dispatch(
          updateUserDetails({
            roles: response.account.idTokenClaims?.roles
          })
        );
      }
      return response.accessToken;
    } catch (error) {
      console.error("Error acquiring token:", error);
    }
  }
  return null;
};
