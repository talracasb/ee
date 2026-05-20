#!/bin/sh

make
./main map > ./json/memorymap.json
cd python
python map.py ../json/memorymap.json