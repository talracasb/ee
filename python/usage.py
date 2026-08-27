import csv
import matplotlib.pyplot as plt
from typing import TypedDict
from pathlib import Path
import sys

class Trial(TypedDict):
    operations: list[int]
    used_memory: list[int]

def read_memory(filename: Path) -> Trial:
    trial: Trial = {
        "operations": [],
        "used_memory": []
    }

    with filename.open(newline="") as file:
        reader = csv.DictReader(file)

        for i, row in enumerate(reader):
            trial["operations"].append(i)
            trial["used_memory"].append(int(row["heap_memory"]))

    return trial

directory = Path(sys.argv[1])
csv_files = sorted(directory.glob("*.csv"))
if not csv_files:
    raise FileNotFoundError(f"No CSV files found in {directory}")

plt.figure(figsize=(12, 6))

for csv_file in csv_files:
    trial = read_memory(csv_file)

    plt.plot(
        trial["operations"],
        trial["used_memory"],
        label=csv_file.stem
    )

plt.xlabel("Operation")
plt.ylabel("Memory (bytes)")
plt.title("Memory Usage Over Time")
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()