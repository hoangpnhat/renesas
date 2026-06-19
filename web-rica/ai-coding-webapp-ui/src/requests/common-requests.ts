import { graphConfig } from "../msal/authConfig";
import { instance } from "./requests";

export const getAccountMe = async () => {
  return await instance.get(graphConfig.graphMeEndpoint, {
    headers: { scopes: ["User.Read"] },
  });
};
export const getAccountPhoto = async () => {
  return await instance.get(graphConfig.graphMePhotEndpoint, {
    headers: { scopes: ["User.Read"] },
    responseType: "blob",
  });
};
