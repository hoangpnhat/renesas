import createSvgIcon from "@mui/material/utils/createSvgIcon";

export const OpenSidebarIcon = createSvgIcon(
  <svg
    xmlns="http://www.w3.org/2000/svg"
    width="20"
    height="20"
    fill="none"
    stroke="currentColor"
    strokeLinecap="round"
    strokeLinejoin="round"
    strokeWidth="1.5"
    className="lucide lucide-panel-right-close"
    viewBox="0 0 24 24"
  >
    <rect width="18" height="18" x="3" y="3" rx="2"></rect>
    <path d="M15 3v18M8 9l3 3-3 3"></path>
  </svg>,
  "OpenSidebarIcon",
);
