import fs from "fs";

import { open } from "sqlite";
import sqlite3 from "sqlite3";

import { DatabaseConnection } from "../indexing/refreshIndex.js";

import { getDevDataSqlitePath } from "../util/paths.js";

/* The Dev Data SQLITE table is only used for local tokens generated */
export class DevDataSqliteDb {
  static db: DatabaseConnection | null = null;

  private static async createTables(db: DatabaseConnection) {
    await db.exec(
      `CREATE TABLE IF NOT EXISTS tokens_generated (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            model TEXT NOT NULL,
            provider TEXT NOT NULL,
            tokens_generated INTEGER NOT NULL,
            tokens_prompt INTEGER NOT NULL DEFAULT 0,
            model_id TEXT,
            synced INTEGER DEFAULT 0,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )`,
    );

    // Add tokens_prompt column if it doesn't exist
    const columnCheckResult = await db.all(
      "PRAGMA table_info(tokens_generated);",
    );
    const tokensPromptExists = columnCheckResult.some(
      (col: any) => col.name === "tokens_prompt",
    );
    if (!tokensPromptExists) {
      await db.exec(
        "ALTER TABLE tokens_generated ADD COLUMN tokens_prompt INTEGER NOT NULL DEFAULT 0;",
      );
    }

    // Add model_id column if it doesn't exist
    const modelIdExists = columnCheckResult.some(
      (col: any) => col.name === "model_id",
    );
    if (!modelIdExists) {
      await db.exec(
        "ALTER TABLE tokens_generated ADD COLUMN model_id TEXT;",
      );
    }

    // Add synced column if it doesn't exist
    const syncedExists = columnCheckResult.some(
      (col: any) => col.name === "synced",
    );
    if (!syncedExists) {
      await db.exec(
        "ALTER TABLE tokens_generated ADD COLUMN synced INTEGER DEFAULT 0;",
      );
    }
  }

  public static async logTokensGenerated(
    model: string,
    provider: string,
    promptTokens: number,
    generatedTokens: number,
    modelId?: string,
  ) {
    const db = await DevDataSqliteDb.get();
    await db?.run(
      "INSERT INTO tokens_generated (model, provider, tokens_prompt, tokens_generated, model_id) VALUES (?, ?, ?, ?, ?)",
      [model, provider, promptTokens, generatedTokens, modelId ?? null],
    );
  }

  public static async getTokensPerDay() {
    const db = await DevDataSqliteDb.get();
    const result = await db?.all(
      // Return a sum of tokens_generated and tokens_prompt columns aggregated by day
      `SELECT date(timestamp) as day, sum(tokens_prompt) as promptTokens, sum(tokens_generated) as generatedTokens
        FROM tokens_generated
        GROUP BY date(timestamp)`,
    );
    return result ?? [];
  }

  public static async getTokensPerModel() {
    const db = await DevDataSqliteDb.get();
    const result = await db?.all(
      // Return a sum of tokens_generated and tokens_prompt columns aggregated by model
      `SELECT model, sum(tokens_prompt) as promptTokens, sum(tokens_generated) as generatedTokens
        FROM tokens_generated
        GROUP BY model`,
    );
    return result ?? [];
  }

  /**
   * Get unsynced token consumption records for RICA dynamic config sync
   * Returns individual records from the last 5 minutes that haven't been synced yet
   */
  public static async getUnsyncedTokenRecords(): Promise<{
    id: number;
    model_id: string;
    tokens_in: number;
    tokens_out: number;
    timestamp: string;
  }[]> {
    const db = await DevDataSqliteDb.get();
    const result = await db?.all(
      `SELECT id, model_id, tokens_prompt as tokens_in, tokens_generated as tokens_out, timestamp
        FROM tokens_generated
        WHERE model_id IS NOT NULL
          AND synced = 0
          AND tokens_generated != 0
        ORDER BY timestamp ASC`,
    );
    return result ?? [];
  }

  /**
   * Mark token records as synced after successfully sending to backend
   */
  public static async markRecordsAsSynced(recordIds: number[]) {
    if (recordIds.length === 0) return;

    const db = await DevDataSqliteDb.get();
    const placeholders = recordIds.map(() => '?').join(',');
    await db?.run(
      `UPDATE tokens_generated SET synced = 1 WHERE id IN (${placeholders})`,
      recordIds,
    );
    console.log(`DevDataSqliteDb: Marked ${recordIds.length} records as synced`);
  }

  static async get() {
    const devDataSqlitePath = getDevDataSqlitePath();
    if (DevDataSqliteDb.db && fs.existsSync(devDataSqlitePath)) {
      return DevDataSqliteDb.db;
    }

    DevDataSqliteDb.db = await open({
      filename: devDataSqlitePath,
      driver: sqlite3.Database,
    });

    await DevDataSqliteDb.db.exec("PRAGMA busy_timeout = 3000;");

    await DevDataSqliteDb.createTables(DevDataSqliteDb.db!);

    return DevDataSqliteDb.db;
  }
}
