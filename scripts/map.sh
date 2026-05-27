#!/bin/sh

cd c
make
./main map > ../json/memorymap.json
echo "rendering image..."
cd ../python
python map.py ../json/memorymap.json