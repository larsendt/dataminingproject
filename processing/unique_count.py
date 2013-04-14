#!/usr/bin/env python
import gzip
import os
import time

prefix = "../data/5grams"
file_list = [f for f in os.listdir(prefix) if f.endswith(".gz")]

current_gram = ""
count = 0
timestamp = time.time()
for fname in file_list:
    f = gzip.open(prefix + "/" + fname, "rb")
    for line in f:
        gram = line[:20]
        if gram != current_gram:
            count += 1
            current_gram = gram
            if count % 1e4 == 0:
                newtime = time.time()
                print "Unique grams:", count, "elapsed time:", newtime - timestamp
                timestamp = newtime


print "Final count of unique grams:", count
