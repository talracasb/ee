import argparse
import json
from pathlib import Path
from typing import TypedDict
import os

import matplotlib.pyplot as plt


class Block(TypedDict):
    event: str
    block_addr: str
    meta_size: int
    user_addr: str
    size: int
    free: bool
    next_block: str
    magic: str
    used: str


def read_memory_map(filename: Path) -> list[Block]:
    with filename.open("r", encoding="utf-8") as file:
        return json.load(file)


def plot_memory_map(filename: Path, max_size: int) -> None:
    blocks = read_memory_map(filename)

    if not blocks:
        raise ValueError(f"{filename} contains no blocks")

    total_memory = sum(block["size"] for block in blocks)

    if total_memory <= 0:
        raise ValueError(f"{filename} has zero total memory size")

    free_positions = []
    free_sizes = []
    used_positions = []
    used_sizes = []

    position = 0

    for block in blocks:
        block_size = block["size"]

        # Keep the real memory position, even for filtered blocks.
        center = position + block_size / 2
        relative_position = center / total_memory * 100

        if block_size <= max_size:
            if block["free"]:
                free_positions.append(relative_position)
                free_sizes.append(block_size)
            else:
                used_positions.append(relative_position)
                used_sizes.append(block_size)

        position += block_size

    fig, ax = plt.subplots(figsize=(12, 6))

    ax.scatter(
        free_positions,
        free_sizes,
        s=25,
        alpha=0.7,
        color="green",
        label="Free"
    )

    ax.scatter(
        used_positions,
        used_sizes,
        s=25,
        alpha=0.7,
        color="red",
        label="Used"
    )


    ax.set_xlim(0, 100)
    ax.set_ylim(0, max_size)

    ax.set_xlabel("Relative Memory Position (%)", fontsize=16)
    ax.set_ylabel("Block Size (bytes)", fontsize=16)
    ax.legend(fontsize=16, loc="upper left")
    ax.set_title(
        f"Memory Block Distribution — {filename.stem.capitalize()} Fit",
        fontsize=22,
    )

    ax.tick_params(axis="both", labelsize=12)

    ax.grid(True, alpha=0.3)

    plt.tight_layout()
    path = Path("graphs/distributions")
    path.mkdir(parents=True, exist_ok=True)

    fig.savefig(
        path / f"{filename.stem}.svg",
        dpi=300,
        bbox_inches="tight",
    )


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Plot memory block distributions from JSON memory maps."
    )

    parser.add_argument(
        "directory",
        type=Path,
        help="Directory containing exactly four JSON memory maps.",
    )

    parser.add_argument(
        "--max-size",
        type=int,
        default=5000,
        help="Ignore blocks larger than this size in the plot (default: 5000).",
    )

    args = parser.parse_args()

    files = sorted(args.directory.glob("*.json"))

    if len(files) != 4:
        raise ValueError(
            f"Expected exactly 4 JSON files in {args.directory}, found {len(files)}"
        )

    for filename in files:
        plot_memory_map(filename, args.max_size)


if __name__ == "__main__":
    main()