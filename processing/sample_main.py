#!/usr/bin/env python

import gzip

fname = "../data/5grams/googlebooks-eng-us-all-5gram-20120701-th.gz"
f = gzip.open(fname, "r")

#grams = {}
#current_gram = ""
count = 0
for line in f:
#    gram = " ".join(line.split()[:-3])
#    occurances = int(line.split()[-1])
    count = count + 1
#    if current_gram == gram:
#        grams[gram].append(occurances)
#    else:
#        current_gram = gram
#        if gram not in grams:
#            grams[gram] = [occurances]
    if(count % 10000000 == 9999999):
        print "10,000,000 lines complete"

f.close()

#print max(grams.items(), key = lambda x: sum(x[1]))
print str(fname) + " " + str(count);
