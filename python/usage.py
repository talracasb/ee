import argparse
import csv
import matplotlib.pyplot as plt
from typing import TypedDict
from pathlib import Path


COLUMNS = {
    "used_memory": {
        "title": "Used Memory Over Time",
        "ylabel": "Used Memory (bytes)",
    },
    "heap_memory": {
        "title": "Heap Memory Usage Over Time",
        "ylabel": "Heap Memory (bytes)",
    },
    "free_blocks": {
        "title": "Free Blocks Over Time",
        "ylabel": "# of Free Blocks",
    },
    "external_fragmentation": {
        "title": "External Fragmentation Over Time",
        "ylabel": "External Fragmentation (%)",
    },
    "internal_fragmentation": {
        "title": "Internal Fragmentation Over Time",
        "ylabel": "Internal Fragmentation (bytes)",
    },
}


class Trial(TypedDict):
    operations: list[int]
    variable: list[float]


def read_memory(filename: Path, column: str) -> Trial:
    trial: Trial = {
        "operations": [],
        "variable": [],
    }

    with filename.open(newline="") as file:
        reader = csv.DictReader(file)

        for i, row in enumerate(reader):
            trial["operations"].append(i)
            trial["variable"].append(float(row[column]))

    return trial


parser = argparse.ArgumentParser(description="Plot memory statistics from CSV files")
parser.add_argument(
    "directory",
    type=Path,
    help="Directory containing CSV files",
)
parser.add_argument(
    "column",
    choices=COLUMNS,
    help="CSV column to plot",
)

args = parser.parse_args()

csv_files = sorted(args.directory.glob("*.csv"))
if not csv_files:
    raise FileNotFoundError(f"No CSV files found in {args.directory}")

config = COLUMNS[args.column]

plt.figure(figsize=(12, 6))

for csv_file in csv_files:
    trial = read_memory(csv_file, args.column)

    plt.plot(
        trial["operations"],
        trial["variable"],
        label=csv_file.stem,
    )

plt.xlabel("Operation")
plt.ylabel(config["ylabel"])
plt.title(config["title"])
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()