#!/bin/bash

# modify the filename to match the set you want to download

while read line; do
    wget $line
done < "5grams.txt"
