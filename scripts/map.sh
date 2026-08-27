#!/bin/sh

cd c
make
./main map first > ../data/memorymap.json
echo "rendering image..."
cd ../python
python map.py ../data/memorymap.json