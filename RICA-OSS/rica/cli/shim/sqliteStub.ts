/**
 * SQLite Stub for RICA CLI
 *
 * The CLI doesn't use sqlite3 (only needed for VS Code indexing features).
 * This stub prevents the native binding from being required at runtime.
 */

export const DevDataSqliteDb = {
  getUnsyncedTokenRecords: async () => [],
  markRecordsAsSynced: async () => {},
  insertTokenRecord: async () => {},
};

export default {};
