#!/usr/bin/env python

import gzip
import os

for fname in os.listdir("../data/5grams/"):
    f = gzip.open("../data/5grams/" + str(fname), "r")

    grams = {}
    current_gram = ""
    count = 0
    for line in f:
        count = count + 1;
        gram = " ".join(line.split()[:-3])
        occurances = int(line.split()[-1])
        if current_gram == gram:
            grams[gram].append(occurances)
        else:
            current_gram = gram
            if gram not in grams:
                grams[gram] = [occurances]

        
        # Print something to keep connection open
        if (count % 10000000 == 9999999):
            print "10,000,000 lines done"
    f.close()

    print max(grams.items())
    print str(fname) + " " + str(count)
