import sys
import json
from typing import TypedDict
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

matplotlib.use("QtAgg")

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

def read() -> list[Block]:
    if len(sys.argv) < 2:
        raise SystemExit("usage: python map.py <file.json>")

    path = sys.argv[1]
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)

def size(block: Block) -> int:
    return max(block["size"], 20)

def draw(data: list[Block]):
    margin = 1;
    height = 1;

    fig, ax = plt.subplots(figsize=(10, 2))
    ax.set_ylim(0, height + margin * 2)
    ax.set_xlim(0, 32 + 2 * margin)
    ax.axis("off")

    total_size = sum(size(block) for block in data)
    scale = 32 / total_size

    x = margin
    y = margin
    for block in data:
        width = size(block) * scale
        ax.add_patch(
            Rectangle((x, y),
                      width,
                      height,
                      fill=False,
                      edgecolor="black",
                      hatch="//" if block["free"] else ""
                      ))
        
        used = f"\nused: {block["used"]}"
        text = f"{block["magic"]}\nsize: {block["size"]}{used if not block["free"] else ""}\n"
        ax.text(x+width / 2, y+height / 2, text, ha="center", va="center", fontsize=6)

        x += width

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    data = read()
    draw(data)

    fragmentation = 0
    small_fractions = 0
    total = 0
    for block in data:
        if not block["free"]:
            fragmentation += block["size"] - block["used"]
        elif block["size"] <= 16:
            small_fractions += block["size"]

        total += block["size"]
    
    print(f"Fragmentation: {fragmentation}/{total}, {fragmentation/total * 100}%")
    print(f"Small Fragments: {small_fractions}/{total}, {small_fractions/total * 100}%")
    