#! /usr/bin/python

from shutil import copyfile
from subprocess import check_output
from random import randint
import os
import time, datetime

N_keys = 100
i = 1

print str(N_keys) + " tests will be executed\n"

while i <= N_keys:
    try:
        size_of_tst_file = randint(1024,102400) # 1 B - 100 B
        print "Run encdectest #" + str(i) + " with size " + str(size_of_tst_file) + "B\n"
        
        start_time = time.time()
        check_output(["./tests/encdectest/build/encdectest", str(size_of_tst_file), "./tests/keys/" + str(i)])
        end_time = time.time()
        
        total_time = end_time - start_time
        
        print "\nTest #" + str(i) + " finished successfully in " + str(time.strftime("%M:%S", time.gmtime(total_time)))
        print "\n**************************************\n"
        
    except:
        print "\nTest #" + str(i) + " failed"
        print "\n**************************************\n"
        exit(-1)
    
    i += 1
