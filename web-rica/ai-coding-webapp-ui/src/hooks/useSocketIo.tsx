/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { io, ManagerOptions, SocketOptions } from "socket.io-client";
import { environment } from "../environments/environment.dev";
import { acquireToken } from "../msal/auth.tsx";

// TODO: handle other event when chatting

const acquireAndSetupToken = async () => {
  // let token: string | null = "";
  // acquireToken().then((_token) => {
  //   token = _token as string;
  // });

  const token = await acquireToken();

  // console.log("token: ", token);

  const socketOpt: Partial<ManagerOptions & SocketOptions> = {
    autoConnect: false,
    transports: ["websocket", "polling"],
    auth: {
      token: `${token}`, // Sử dụng token đã lấy
    },
    extraHeaders: {
      Authorization: `Bearer ${token}`, // Sử dụng cùng một token
    },
  };

  return socketOpt;
};

export const initializeSocket = async () => {
  const socketOpt = await acquireAndSetupToken();
  // const socketOpt: Partial<ManagerOptions & SocketOptions> = {
  //   autoConnect: false,
  //   transports: ["websocket", "polling"],
  // };

  return environment.mode === "development"
    ? io(environment.baseURLSocket, socketOpt)
    : io(socketOpt);
};

// Lazy initialization of socket
let socketInstance: Awaited<ReturnType<typeof initializeSocket>> | null = null;
let socketPromise: Promise<
  Awaited<ReturnType<typeof initializeSocket>>
> | null = null;

export const getSocket = () => {
  if (socketInstance) {
    return Promise.resolve(socketInstance);
  }

  if (!socketPromise) {
    socketPromise = initializeSocket().then((socket) => {
      socketInstance = socket;
      return socket;
    });
  }

  return socketPromise;
};
