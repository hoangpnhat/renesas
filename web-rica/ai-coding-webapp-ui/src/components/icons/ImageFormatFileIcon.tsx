/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */

import createSvgIcon from "@mui/material/utils/createSvgIcon";
import type { SvgIconProps } from "@mui/material/SvgIcon";

export enum ImageFormat {
  JPG = "jpg",
  PNG = "png",
  GIF = "gif",
  BMP = "bmp",
  WEBP = "webp",
  TIFF = "tiff",
  SVG = "svg",
  HEIC = "heic",
  HEIF = "heif",
  JPEG = "jpeg",
}

export interface ImageFormatFileIconProps extends SvgIconProps {
  format: ImageFormat;
  size?: number;
}

const ImageFormatIcon = createSvgIcon(
  <g>
    {/* File outline */}
    <path
      d="M14,2H6A2,2 0 0,0 4,4V20A2,2 0 0,0 6,22H18A2,2 0 0,0 20,20V8L14,2Z"
      fill="currentColor"
      opacity="0.1"
    />
    <path
      d="M14,2H6A2,2 0 0,0 4,4V20A2,2 0 0,0 6,22H18A2,2 0 0,0 20,20V8L14,2Z"
      fill="none"
      stroke="currentColor"
      strokeWidth="1.5"
    />

    {/* File fold corner */}
    <path d="M14,2V8H20" fill="none" stroke="currentColor" strokeWidth="1.5" />

    {/* Image preview area */}
    <rect
      x="7"
      y="10"
      width="10"
      height="6"
      rx="1"
      fill="none"
      stroke="currentColor"
      strokeWidth="1"
      opacity="0.6"
    />

    {/* Mountain/landscape icon inside preview */}
    <path
      d="M9,14L10.5,12L12,13.5L14.5,11L16,12.5V15H8V14Z"
      fill="currentColor"
      opacity="0.4"
    />

    {/* Sun icon */}
    <circle cx="14" cy="12" r="1" fill="currentColor" opacity="0.4" />
  </g>,
  "ImageFormat",
);

// Component that accepts format as props
export const ImageFormatFileIcon = ({
  format = ImageFormat.JPG,
  size = 24,
  color = "inherit",
  ...props
}: ImageFormatFileIconProps) => {
  return (
    <div style={{ position: "relative", display: "inline-block" }}>
      <ImageFormatIcon
        style={{
          fontSize: size,
          color: color,
          display: "block",
        }}
        {...props}
      />
      {/* Format text overlay */}
      <div
        style={{
          position: "absolute",
          bottom: "2px",
          left: "50%",
          transform: "translateX(-50%)",
          fontSize: Math.max(8, size * 0.25),
          fontFamily: "monospace",
          fontWeight: "bold",
          color: color,
          textAlign: "center",
          lineHeight: 1,
          textTransform: "uppercase",
          maxWidth: `${size * 0.8}px`,
          overflow: "hidden",
          whiteSpace: "nowrap",
        }}
      >
        {format.slice(0, 4).toUpperCase()}
      </div>
    </div>
  );
};
