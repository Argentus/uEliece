#! /usr/bin/python

from shutil import copyfile, rmtree
from subprocess import call
import os

N_keys = 100
i = 1

try:
    rmtree("keys")
except:
    pass
        
while i <= N_keys:
    call(["mkdir", "keys"])        
    call(["../keygen/uEl-keygen", "-B"])
    
    copyfile("uEl_pub.key", "keys/" + str(i) + ".pub")
    copyfile("uEl_priv.key", "keys/" + str(i) + ".priv")
    
    os.remove("uEl_pub.key")
    os.remove("uEl_priv.key")
    
    i += 1
