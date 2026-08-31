#!/bin/sh

mkdir -p data/memory_maps
cd c
make
./main map first > ../data/memory_maps/first.json
./main map best > ../data/memory_maps/best.json
./main map worst > ../data/memory_maps/worst.json
./main map next > ../data/memory_maps/next.json
echo "rendering image..."
cd ..
python python/distribution.py ./data/memory_maps