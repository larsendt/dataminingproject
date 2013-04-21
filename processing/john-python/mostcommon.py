#!/usr/bin/env python

import gzip
import os
import re

fmeta = open("../data/5grams-info/eng-us-all-totalcounts.txt", "r")
fmetalines = fmeta.readlines()
ls = fmetalines[0].split("\t")
gpy = {}
for item in ls:
    ls = item.split(",")
    if(len(ls) > 1):
        gpy[ls[0]] = ls[1]

files = 0
for fname in os.listdir("../data/pos/5grams/"):
    f = gzip.open("../data/pos/5grams/" + str(fname), "r")
    files = files + 1

    grams = {}
    current_gram = ""
    lcount = 0
    for line in f:
        lcount = lcount + 1;
        gram = " ".join(line.split()[:-3])
        occurances = int(line.split()[-1])
        year = str(line.split()[-3])
        occurfreq = occurances/float(gpy[year])
        
#        grams[gram][year] = occurfreq
        if current_gram == gram:
            grams[gram].append(occurfreq)
        else:
            current_gram = gram
            if gram not in grams:
                grams[gram] = [occurfreq]
        
        # Print something to keep connection open
        if (lcount % 10000000 == 9999999):
            print "10,000,000 lines done"

#    print grams[str(gram)]
    print max(grams.items(), key = lambda x: sum(x[1]))

    # Only run for two files
    #if(files > 2):
        #break

f.close()


#    print "Count yes " + str(count_yes)
#    print "Count no " + str(count_no)
#    print "Count " + str(count)
