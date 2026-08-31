#!/bin/sh

cd c
make clean
make
rm -rf ../data/usage
mkdir -p ../data/usage
echo "first"
./main usage first > ../data/usage/first.csv
echo "best"
./main usage best > ../data/usage/best.csv
echo "worst"
./main usage worst > ../data/usage/worst.csv
echo "next"
./main usage next > ../data/usage/next.csv
echo "done!"
echo "rendering image..."
cd ..
python python/usage.py ./data/usage "heap_memory"
python python/usage.py ./data/usage "internal_fragmentation"
python python/usage.py ./data/usage "external_fragmentation"
python python/usage.py ./data/usage "free_blocks"