import { logout as ricaLogout } from "../auth/workos.js";

export async function logout() {
  ricaLogout();
}
