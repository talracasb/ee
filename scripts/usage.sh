#!/bin/sh

cd c
make clean
make
rm -rf ../data/usage
mkdir -p ../data/usage
./main usage first > ../data/usage/first.csv
./main usage best > ../data/usage/best.csv
echo "rendering image..."
cd ../python
python usage.py ../data/usage