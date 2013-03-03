#!/usr/bin/env python

import re

REGEXES = ["(googlebooks-eng-us-all-1gram-20120701-([a-z_]|[0-9]|_ADJ_|_ADP_|_ADV_|_CONJ_|_DET_|_NOUN_|_NUM_|_PRON_|_PRT_|_VERB_)\.gz)",
            "(googlebooks-eng-us-all-2gram-20120701-([a-z_][a-z_]|[0-9]|_ADJ_|_ADP_|_ADV_|_CONJ_|_DET_|_NOUN_|_NUM_|_PRON_|_PRT_|_VERB_)\.gz)",
            "(googlebooks-eng-us-all-3gram-20120701-([a-z_][a-z_]|[0-9]|_ADJ_|_ADP_|_ADV_|_CONJ_|_DET_|_NOUN_|_NUM_|_PRON_|_PRT_|_VERB_)\.gz)",
            "(googlebooks-eng-us-all-4gram-20120701-([a-z_][a-z_]|[0-9]|_ADJ_|_ADP_|_ADV_|_CONJ_|_DET_|_NOUN_|_NUM_|_PRON_|_PRT_|_VERB_)\.gz)",
            "(googlebooks-eng-us-all-5gram-20120701-([a-z_][a-z_]|[0-9]|_ADJ_|_ADP_|_ADV_|_CONJ_|_DET_|_NOUN_|_NUM_|_PRON_|_PRT_|_VERB_)\.gz)"]

def main():
    for idx, rgx in enumerate(REGEXES):
        prefix = "http://storage.googleapis.com/books/ngrams/books/%s"
        text = open("datasetsv2.html", "r").read() 
        m = re.findall(rgx, text)
        matches = map(lambda x: prefix % x[0], m)
        f = open("%dgrams.txt" % (idx+1), "w")
        f.write("\n".join(matches))
        f.close()

if __name__ == "__main__": main()
