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
    
    copyfile("uEl_pub.key", "keys/uEl_pub_" + str(i) + ".key")
    copyfile("uEl_priv.key", "keys/uEl_priv_" + str(i) + ".key")
    
    os.remove("uEl_pub.key")
    os.remove("uEl_priv.key")
    
    i += 1
