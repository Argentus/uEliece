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
        if i <= 20:
            size_of_tst_file = 10240 # 10B
        elif i > 20 and i <= 40:
            size_of_tst_file = 51200 # 50B
        elif i > 40 and i <= 60:
            size_of_tst_file = 512000 # 500B
        elif i > 60 and i <= 80:
            size_of_tst_file = 1024000 # 1MB
        else:
            size_of_tst_file = 10240000 # 10MB        

        print "Run encdectest #" + str(i) + " with size " + str(size_of_tst_file) + "B and key '" + str((i % 4) + 1)+"'\n"
        
        start_time = time.time()
        check_output(["./tests/encdectest/build/encdectest", str(size_of_tst_file), "./tests/keys/" + str((i % 4) + 1)])
        end_time = time.time()
        
        total_time = end_time - start_time
        
        print "\nTest #" + str(i) + " finished successfully in " + str(time.strftime("%M:%S", time.gmtime(total_time)))
        print "\n**************************************\n"
        
    except:
        print "\nTest #" + str(i) + " failed"
        print "\n**************************************\n"
        exit(-1)
    
    i += 1
