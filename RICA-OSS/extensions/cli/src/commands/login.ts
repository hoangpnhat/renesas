import chalk from "chalk";

import { login as ricaLogin } from "../auth/workos.js";
import { gracefulExit } from "../util/exit.js";
// RICA overlay: branding strings
import { cliStrings } from "../../../../../rica/cli/branding/cliStrings.js";

import { chat } from "./chat.js";

export async function login() {
  console.info(chalk.yellow(cliStrings.loginMessage));

  try {
    await ricaLogin();
    console.info(chalk.green(cliStrings.loginSuccess));

    console.info(chalk.blue(cliStrings.startingCli));
    await chat();
  } catch (error: any) {
    console.error(chalk.red(`Login failed: ${error.message}`));
    await gracefulExit(1);
  }
}
