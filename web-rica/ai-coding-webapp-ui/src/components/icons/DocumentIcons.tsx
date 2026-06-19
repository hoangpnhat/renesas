/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import { createSvgIcon } from "@mui/material/utils";

export const MdIcons = createSvgIcon(
  <svg
    xmlns="http://www.w3.org/2000/svg"
    width="120"
    height="80"
    fill="none"
    viewBox="0 0 120 80"
  >
    <rect width="120" height="80" fill="#2E86C1" rx="12"></rect>
    <path
      fill="#fff"
      d="M20 60V20h15l15 20 15-20h15v40H65V35L50 50 35 35v25zM90 50l15 15 15-15h-10V35h-10v15z"
    ></path>
  </svg>,
  "DocumentIcons",
);

export const ExcelIcon = createSvgIcon(
  <svg
    xmlns="http://www.w3.org/2000/svg"
    width="666.667"
    height="666.667"
    fillRule="evenodd"
    clipRule="evenodd"
    imageRendering="optimizeQuality"
    shapeRendering="geometricPrecision"
    textRendering="geometricPrecision"
    viewBox="0 0 17639 17639"
  >
    <path
      fill="#1e7145"
      d="M9596 3634v10265l-6103-1058V4692l6103-1058zm290 1185h4053c224 0 267 44 267 267v7360c0 223-43 267-267 267H9886v-694h1547v-1066H9886v-320h1547V9566H9886v-320h1547V8179H9886v-320h1547V6793H9886v-320h1547V5406H9886v-587zM2083 15593h13474l2-13476H2081l2 13476z"
    ></path>
    <path
      fill="#1e7145"
      d="M7129 6861l-556 1272-432-1195-695 31 690 1741-777 1703 683 51 548-1225 510 1296 786 43-852-1875 815-1894zm4624 5158h1760v-1066h-1760zm0-1386h1760V9566h-1760zm0-1387h1760V8179h-1760zm0-2773h1760V5406h-1760zm0 1386h1760V6793h-1760z"
    ></path>
  </svg>,
  "ExcelIcon",
);

export const CsvIcon = createSvgIcon(
  <svg
    xmlns="http://www.w3.org/2000/svg"
    width="100"
    height="100"
    viewBox="0 0 64 64"
  >
    <path fill="#dee2e6" d="M16 8h24l8 8v32H16z"></path>
    <path fill="#bdc3c7" d="M48 16h-8V8z"></path>
    <rect width="24" height="3" x="20" y="24" fill="#34a853" rx="1.5"></rect>
    <rect width="24" height="3" x="20" y="30" fill="#34a853" rx="1.5"></rect>
    <rect width="24" height="3" x="20" y="36" fill="#34a853" rx="1.5"></rect>
    <path fill="#34a853" d="M8 40h48v16H8z"></path>
    <text
      x="16"
      y="54"
      fill="#fff"
      fontFamily="Arial"
      fontSize="14"
      fontWeight="bold"
    >
      CSV
    </text>
  </svg>,
  "CsvIcon",
);

export const TxtIcon = createSvgIcon(
  <svg
    xmlns="http://www.w3.org/2000/svg"
    width="100"
    height="100"
    viewBox="0 0 64 64"
  >
    <path fill="#dee2e6" d="M16 8h24l8 8v32H16z"></path>
    <path fill="#bdc3c7" d="M48 16h-8V8z"></path>
    <rect width="24" height="3" x="20" y="24" fill="#4285F4" rx="1.5"></rect>
    <rect width="24" height="3" x="20" y="30" fill="#4285F4" rx="1.5"></rect>
    <rect width="24" height="3" x="20" y="36" fill="#4285F4" rx="1.5"></rect>
    <path fill="#4285F4" d="M8 40h48v16H8z"></path>
    <text
      x="16"
      y="54"
      fill="#fff"
      fontFamily="Arial"
      fontSize="14"
      fontWeight="bold"
    >
      TXT
    </text>
  </svg>,
  "TxtIcon",
);

export const JsonIcon = createSvgIcon(
  <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
    <path
      fill="#ECECEC"
      d="M128 0c-17.7 0-32 14.3-32 32v448c0 17.7 14.3 32 32 32h256c17.7 0 32-14.3 32-32V128L288 0z"
    ></path>
    <path fill="#D1D1D1" d="M384 128h-96V32z"></path>
    <path
      fill="#666"
      d="M168 240c-7 0-10 3-10 9v17c0 6-2 9-8 10v8c6 1 8 4 8 10v17c0 6 3 9 10 9v-7c-4 0-5-2-5-7v-17c0-5-3-8-7-9 4-1 7-4 7-9v-17c0-5 1-7 5-7zM344 240c7 0 10 3 10 9v17c0 6 2 9 8 10v8c-6 1-8 4-8 10v17c0 6-3 9-10 9v-7c4 0 5-2 5-7v-17c0-5 3-8 7-9-4-1-7-4-7-9v-17c0-5-1-7-5-7z"
    ></path>
    <path fill="#E74C3C" d="M194 260h30v10h-30ZM234 260h30v10h-30Z"></path>
    <path fill="#3498DB" d="M194 290h90v10h-90Z"></path>
    <path fill="#27AE60" d="M294 290h30v10h-30Z"></path>
    <path
      fill="#E74C3C"
      d="M306 400c0-11.05 8.95-20 20-20h20c11.05 0 20 8.95 20 20v20c0 11.05-8.95 20-20 20h-20c-11.05 0-20-8.95-20-20Z"
    ></path>
    <text
      x="336"
      y="420"
      fill="#fff"
      fontFamily="Arial, sans-serif"
      fontSize="24"
      fontWeight="bold"
      textAnchor="middle"
    >
      J
    </text>
  </svg>,
  "JsonIcon",
);
export const HTMLIcon = createSvgIcon(
  <svg
    xmlns="http://www.w3.org/2000/svg"
    xmlSpace="preserve"
    id="Layer_1"
    version="1.1"
    viewBox="0 0 512 512"
  >
    <path fill="#FFF" d="M67.544 4h376.96v504H67.544z"></path>
    <path
      fill="#AAC1CE"
      d="M440.456 8v496H71.544V8zm8-8H63.544v512h384.912z"
    ></path>
    <g fill="#415E72">
      <path d="m130.576 117.512 85.6-39.752v18.896L151.464 125v.36l64.712 28.344V172.6l-85.6-39.752zM229.352 179.728 267.504 50.48h18.008L247.36 179.728zM381.424 133.376l-85.6 39.2v-18.872L362 125.36V125l-66.136-28.344V77.76l85.6 39.2z"></path>
    </g>
    <g fill="#32BEA6">
      <path d="M136.056 301.352h-18.704l20-95.528h18.704l-7.2 34.016a49.3 49.3 0 0 1 11.8-7.136 31.3 31.3 0 0 1 11.736-2.184 18.06 18.06 0 0 1 13.288 4.8 16.94 16.94 0 0 1 4.928 12.68 70 70 0 0 1-1.832 12.896l-8.472 40.472H161.6l8.664-41.248a58 58 0 0 0 1.368-8.6 7.34 7.34 0 0 0-2.152-5.536 8.05 8.05 0 0 0-5.864-2.088c-3.28.056-6.44 1.2-8.992 3.256a28.3 28.3 0 0 0-8.528 10.552 94.7 94.7 0 0 0-4.304 16.224zM203.312 246.024l2.864-13.872h9.12l2.28-11.08 21.44-12.904-5.016 24h11.408l-2.864 13.872h-11.472l-6.064 29a90 90 0 0 0-1.6 8.992 4.23 4.23 0 0 0 1.368 3.36 7.68 7.68 0 0 0 4.952 1.2c.824 0 2.88-.152 6.184-.456L232.96 302a49 49 0 0 1-9.904.984 22.14 22.14 0 0 1-14.4-3.816 13.15 13.15 0 0 1-4.496-10.592 100 100 0 0 1 2.4-14.728l5.8-27.832zM256.944 232.152h17.6l-1.744 8.408a30.91 30.91 0 0 1 21.6-10.04 19.42 19.42 0 0 1 12.056 3.256 14.6 14.6 0 0 1 5.408 8.992 27.85 27.85 0 0 1 10.296-8.728 29.6 29.6 0 0 1 13.944-3.52 17.93 17.93 0 0 1 12.904 4.432 15.56 15.56 0 0 1 4.624 11.736 72 72 0 0 1-1.832 12.384l-8.84 42.28h-18.696l8.856-42.288c1.064-5.304 1.6-8.304 1.6-8.992a6 6 0 0 0-1.728-4.528 7.2 7.2 0 0 0-5.176-1.664 15.52 15.52 0 0 0-12.448 7.368 49.7 49.7 0 0 0-6.784 18.704l-6.576 31.408h-18.672l8.728-41.832a61 61 0 0 0 1.504-9.384 5.92 5.92 0 0 0-1.848-4.496 7.17 7.17 0 0 0-5.12-1.76 12.94 12.94 0 0 0-6.32 1.832 18.6 18.6 0 0 0-5.704 5.016 29.4 29.4 0 0 0-4.2 8.208c-.536 1.56-1.376 5.104-2.536 10.624l-6.648 31.8H242.48zM361.6 301.352l20-95.528h18.64l-19.944 95.528z"></path>
    </g>
    <g fill="#AAC1CE">
      <path d="M112 417.92h288v8H112zM112 451.68h288v8H112zM112 384.16h288v8H112zM112 350.44h288v8H112z"></path>
    </g>
  </svg>,
  "HTMLIcon",
);
