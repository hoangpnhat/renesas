export const environment = {
  baseURL: import.meta.env.VITE_BASE_URL || "http://127.0.0.1:8000/",
  mode: import.meta.env.VITE_ENVIRONMENT || "production",
  apimAccessKey:
    import.meta.env.VITE_APIM_ACCESS || "c22678ace1a84e318141251a6fbe145e",
  baseURLSocket:
    import.meta.env.VITE_BASE_URL_SOCKET || "http://127.0.0.1:8000/",
  projectName: import.meta.env.VITE_PROJECT_NAME,
  preferredProjectName: "Renesas Intelligent Coding Assistant.",
  accessToken:
    "eyJhbGciOiJIUzUxMiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MjU4NzgxNjksInN1YiI6IjY2YmY0Yzg2MjcyMjVkODQyMzFmNzk3NCIsInRvdHAiOmZhbHNlfQ.1ywtUq3MgKzgBhI4o6OVW0LIgRZ0pj3F5tiDcawc-CU4mZQx-2RMC8hOlw4YkfcO79qJ5qx8F7B9eHxICkN_yw",
};
