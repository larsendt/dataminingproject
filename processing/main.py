#!/usr/bin/env python

import gzip

fname = "../data/5grams/googlebooks-eng-us-all-5gram-20120701-mk.gz"
f = gzip.open(fname, "r")

grams = {}
current_gram = ""
for line in f:
    gram = " ".join(line.split()[:-3])
    occurances = int(line.split()[-1])
    if current_gram == gram:
        grams[gram].append(occurances)
    else:
        current_gram = gram
        if gram not in grams:
            grams[gram] = [occurances]

f.close()

print max(grams.items(), key = lambda x: sum(x[1]))
