#!/usr/bin/python

import os, sys, re

path = "C:\\dataset\\Color_Neutral_jpg"
dirs = os.listdir( path )

subj_ind = 0
fid = open(path+'\\subjects.txt', 'w')
# This would print all the files and directories
for file in dirs:
    # The subjects file & landmarks dir is in the same directory, don't look at it
    if (file != 'subjects.txt') & (file != 'landmarks'):
        # Find the decimal characters
        m = re.search("\d", file)
        age = file[m.start(0):m.end(0)+1]

        # Find the first lowercase - this will be gender
        for c in file:
            if c.islower():
                gen = c
                break

        fid.write(gen + ' ' + age+'\n')

fid.close()
