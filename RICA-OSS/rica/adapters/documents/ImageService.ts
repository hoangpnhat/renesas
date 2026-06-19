import * as crypto from "crypto";
import * as fs from "fs";
import fetch from "node-fetch";
import * as path from "path";

import { IDE } from "../../../core";
import {
  localPathOrUriToPath,
  localPathToUri,
} from "../../../core/util/pathToUri";
import { joinPathsToUri } from "../../../core/util/uri";
import type { IdeSettings } from "../config/env";
import {
  getDatabricksConfig,
  resolveServiceEnvironment,
  type ServiceEnvironment,
} from "./ServiceEndpoints";

export class ImageService {
  private static readonly REQUEST_TIMEOUT = 30000;
  private static readonly IMAGES_FOLDER = ".rica/images";
  private imageDir: string | null = null;

  constructor(
    private readonly getMdpToken: () => Promise<string | undefined>,
    private readonly ide: IDE,
    private readonly ideSettingsPromise: Promise<IdeSettings>,
  ) {}

  private async getEnvironment(): Promise<ServiceEnvironment> {
    const settings = await this.ideSettingsPromise;
    return resolveServiceEnvironment(settings.continueTestEnvironment);
  }

  private async getDatabricksEndpoint(): Promise<string> {
    const env = await this.getEnvironment();
    return getDatabricksConfig(env).endpoint;
  }

  private async getImageDir(): Promise<string | null> {
    if (this.imageDir) return this.imageDir;

    try {
      const workspaceDirs = await this.ide.getWorkspaceDirs();
      if (workspaceDirs.length === 0) return null;

      const imageDirUri = joinPathsToUri(
        workspaceDirs[0],
        ImageService.IMAGES_FOLDER,
      );
      const imageDirPath = localPathOrUriToPath(imageDirUri);

      if (!fs.existsSync(imageDirPath)) {
        fs.mkdirSync(imageDirPath, { recursive: true });
      }

      this.imageDir = imageDirPath;
      return imageDirPath;
    } catch (error) {
      console.error("ImageService: Failed to get image directory", error);
      return null;
    }
  }

  async fetchImageAsBase64(filePath: string): Promise<string | undefined> {
    try {
      const token = await this.getMdpToken();
      if (!token) {
        console.error("[ImageService] No MDP token available");
        return undefined;
      }

      const endpoint = await this.getDatabricksEndpoint();
      const encodedPath = filePath.split("/").map(encodeURIComponent).join("/");
      const apiUrl = `${endpoint}/api/2.0/fs/files${encodedPath}`;

      const controller = new AbortController();
      const timeoutId = setTimeout(
        () => controller.abort(),
        ImageService.REQUEST_TIMEOUT,
      );

      const response = await fetch(apiUrl, {
        method: "GET",
        headers: { Authorization: `Bearer ${token}` },
        signal: controller.signal,
      });

      clearTimeout(timeoutId);

      if (!response.ok) {
        const errorText = await response.text();
        console.error(`ImageService: HTTP ${response.status} - ${errorText}`);
        return undefined;
      }

      const buffer = Buffer.from(await response.arrayBuffer());
      const contentType = this.getContentType(filePath);
      return `data:${contentType};base64,${buffer.toString("base64")}`;
    } catch (error) {
      console.error("ImageService: Failed to fetch image", error);
      return undefined;
    }
  }

  async fetchImagesAsBase64(filePaths: string[]): Promise<Map<string, string>> {
    const results = new Map<string, string>();

    const promises = filePaths.map(async (filePath) => {
      const base64 = await this.fetchImageAsBase64(filePath);
      if (base64) results.set(filePath, base64);
    });

    await Promise.all(promises);
    return results;
  }

  async fetchImageWithLocalPath(
    filePath: string,
  ): Promise<{ localPath: string; base64: string } | undefined> {
    try {
      const token = await this.getMdpToken();
      if (!token) return undefined;

      const endpoint = await this.getDatabricksEndpoint();
      const encodedPath = filePath.split("/").map(encodeURIComponent).join("/");
      const apiUrl = `${endpoint}/api/2.0/fs/files${encodedPath}`;

      const controller = new AbortController();
      const timeoutId = setTimeout(
        () => controller.abort(),
        ImageService.REQUEST_TIMEOUT,
      );

      const response = await fetch(apiUrl, {
        method: "GET",
        headers: { Authorization: `Bearer ${token}` },
        signal: controller.signal,
      });

      clearTimeout(timeoutId);

      if (!response.ok) return undefined;

      const buffer = Buffer.from(await response.arrayBuffer());

      const localPath = await this.saveImageToWorkspaceFile(filePath, buffer);
      if (!localPath) return undefined;

      const contentType = this.getContentType(filePath);
      const dataUrl = `data:${contentType};base64,${buffer.toString("base64")}`;

      return { localPath, base64: dataUrl };
    } catch (error) {
      console.error(
        "ImageService: Failed to fetch image with local path",
        error,
      );
      return undefined;
    }
  }

  async fetchImagesWithLocalPaths(
    filePaths: string[],
  ): Promise<Map<string, { localPath: string; base64: string }>> {
    const results = new Map<string, { localPath: string; base64: string }>();

    const promises = filePaths.map(async (filePath) => {
      const result = await this.fetchImageWithLocalPath(filePath);
      if (result) results.set(filePath, result);
    });

    await Promise.all(promises);
    return results;
  }

  private async saveImageToWorkspaceFile(
    filePath: string,
    buffer: Buffer,
  ): Promise<string | undefined> {
    try {
      const imageDir = await this.getImageDir();
      if (!imageDir) return undefined;

      const hash = crypto.createHash("md5").update(filePath).digest("hex");
      const ext = path.extname(filePath) || ".png";
      const fileName = `${hash}${ext}`;
      const localPath = path.join(imageDir, fileName);

      fs.writeFileSync(localPath, new Uint8Array(buffer));
      return localPathToUri(localPath);
    } catch (error) {
      console.error(
        "ImageService: Failed to save image to workspace file",
        error,
      );
      return undefined;
    }
  }

  private getContentType(filePath: string): string {
    const lowerPath = filePath.toLowerCase();
    if (lowerPath.endsWith(".jpg") || lowerPath.endsWith(".jpeg"))
      return "image/jpeg";
    if (lowerPath.endsWith(".gif")) return "image/gif";
    if (lowerPath.endsWith(".webp")) return "image/webp";
    if (lowerPath.endsWith(".svg")) return "image/svg+xml";
    return "image/png";
  }
}
