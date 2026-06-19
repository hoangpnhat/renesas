import createSvgIcon from "@mui/material/utils/createSvgIcon";

export const CloseSidebarIcon = createSvgIcon(
  <svg
    xmlns="http://www.w3.org/2000/svg"
    width="20"
    height="20"
    fill="none"
    stroke="currentColor"
    strokeLinecap="round"
    strokeLinejoin="round"
    strokeWidth="1.5"
    className="lucide lucide-panel-left-close"
    viewBox="0 0 24 24"
  >
    <rect width="18" height="18" x="3" y="3" rx="2"></rect>
    <path d="M9 3v18M16 15l-3-3 3-3"></path>
  </svg>,
  "CloseSidebarIcon"
);
