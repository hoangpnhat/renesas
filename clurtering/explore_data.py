"""
Explore and combine the Excel data
"""

import pandas as pd
import json

# Read all sheets
print("Reading Clustering Project.xlsx...")
xls = pd.ExcelFile('Clustering Project.xlsx')

print(f"\nFound {len(xls.sheet_names)} sheets:")
for i, sheet_name in enumerate(xls.sheet_names, 1):
    print(f"  {i}. {sheet_name}")

# Read each sheet
sheets = {}
for sheet_name in xls.sheet_names:
    df = pd.read_excel(xls, sheet_name=sheet_name)
    sheets[sheet_name] = df

    print(f"\n{'='*70}")
    print(f"Sheet: {sheet_name}")
    print(f"{'='*70}")
    print(f"Shape: {df.shape} (rows: {df.shape[0]}, columns: {df.shape[1]})")
    print(f"\nColumns: {list(df.columns)}")
    print(f"\nFirst 3 rows:")
    print(df.head(3))
    print(f"\nData types:")
    print(df.dtypes)
    print(f"\nMissing values:")
    print(df.isnull().sum())

# Save info to file for review
with open('data_exploration.txt', 'w', encoding='utf-8') as f:
    f.write("=" * 70 + "\n")
    f.write("DATA EXPLORATION SUMMARY\n")
    f.write("=" * 70 + "\n\n")

    for sheet_name, df in sheets.items():
        f.write(f"\nSheet: {sheet_name}\n")
        f.write(f"Shape: {df.shape}\n")
        f.write(f"Columns: {list(df.columns)}\n")
        f.write(f"\nSample data:\n")
        f.write(df.head(5).to_string())
        f.write("\n\n" + "="*70 + "\n")

print("\n" + "="*70)
print("Exploration complete! Results saved to data_exploration.txt")
print("="*70)
