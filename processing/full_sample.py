#!/usr/bin/env python

import gzip
import os
import re

for fname in os.listdir("../data/5grams/"):
    f = gzip.open("../data/5grams/" + str(fname), "r")

    count_yes = 0
    count_no = 0
    count = 0
    for line in f:
        #match = re.search( r'(_ADJ)* (_ADP)* (_ADV)* (_CONJ)*', line, re.M|re.I)
        match = re.search( r'(_ADJ)', line, re.M|re.I)
        count = count + 1
        if match:
            count_yes = count_no + 1
        else:
            count_no = count_no + 1
        # Print something to keep connection open
        if (count % 10000000 == 9999999):
            print "10,000,000 lines done"
    f.close()

    print "Count yes " + str(count_yes)
    print "Count no " + str(count_no)
    print "Count " + str(count)
