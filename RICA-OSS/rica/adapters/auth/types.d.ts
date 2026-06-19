// Type declarations for runtime dependencies
// These are not needed for compilation but required for TypeScript type checking

declare module "node-fetch" {
  export default function fetch(
    url: string,
    init?: any,
  ): Promise<{
    ok: boolean;
    status: number;
    text(): Promise<string>;
    json(): Promise<any>;
    arrayBuffer(): Promise<ArrayBuffer>;
  }>;
}

declare module "uuid" {
  export function v4(): string;
}
