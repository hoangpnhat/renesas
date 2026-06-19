# RICA Branding Assets

This directory contains the source-of-truth for all RICA brand assets.

## Structure

- `source/` - Original high-quality assets (PNG, SVG, etc.)
  - These are the canonical source files
  - Edit these files when updating branding
  - Do NOT edit files in `extensions/vscode/media/` directly

## Assets

| File                     | Purpose                             | Used In                |
| ------------------------ | ----------------------------------- | ---------------------- |
| `icon-rica.png`          | Main extension icon (large, 1.5MB)  | `package.json` "icon"  |
| `icon-rica-small.png`    | Small extension icon (8KB)          | Marketplace            |
| `RICA_README.png`        | README banner (1.7MB)               | README.md              |
| `icon-sidebar-icon.png`  | Activity bar icon with text (1.4MB) | Activity bar           |
| `sidebar-icon.png`       | Sidebar icon (70KB)                 | View container         |
| `sidebar-icon-no-bg.png` | Sidebar icon no background (54KB)   | Alternative view       |
| `text-sidebar-icon.png`  | Text-based sidebar icon (421KB)     | Activity bar (current) |

## Build Process

During extension build, these files are copied to `extensions/vscode/media/` where they are referenced by `package.json`.

**Source of truth**: `rica/branding/assets/source/`
**Build output**: `extensions/vscode/media/`

## Updating Assets

1. Edit files in `rica/branding/assets/source/`
2. Copy to `extensions/vscode/media/` (or use build script)
3. Test extension build
4. Commit both locations

## VSCode Package References

See `rica/branding/brand.json` for the canonical asset path mappings used in `package.json` generation.
