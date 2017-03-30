#! /usr/bin/python

from shutil import copyfile
from subprocess import call
from random import randint
import os
import time, datetime

N_keys = len(os.listdir("./tests/keys")) / 2
i = 1

print str(N_keys) + " tests will be executed\n"

while i <= N_keys:
    try:
        copyfile("./tests/keys/uEl_pub_1.key", "uEl_pub.key")
        copyfile("./tests/keys/uEl_priv_1.key", "uEl_priv.key")

        size_of_tst_file = randint(1024,104857600) # 1 B - 100 MB
        print "Run encdectest #" + str(i) + " with size " + str(size_of_tst_file) + " B\n"
        
        start_time = time.time()
        call(["./tests/encdectest/build/encdectest", "-s", str(size_of_tst_file)])
        end_time = time.time()
        
        total_time = end_time - start_time
        
        os.remove("uEl_pub.key")
        os.remove("uEl_priv.key")
        
        print "\nTest #" + str(i) + " finished successfully in " + str(time.strftime("%M:%S", time.gmtime(total_time)))
        print "\n**************************************\n"
        
    except:
        print "\nTest #" + str(i) + " failed"
        print "\n**************************************\n"
        exit(-1)
    
    i += 1
