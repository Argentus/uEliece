#!/bin/bash

#
# 
#

VERSION=$(cat version)

# uEliece.c
THE_FILE="uEliece.c"
head -2 src/$THE_FILE > src/$THE_FILE.new
printf " *	Version: %s\n" "$VERSION" >> src/$THE_FILE.new
tail -n +4 src/$THE_FILE >> src/$THE_FILE.new
mv src/$THE_FILE src/$THE_FILE.old
mv src/$THE_FILE.new src/$THE_FILE
rm src/$THE_FILE.old

# uEliece.h
THE_FILE="uEliece.h"
head -2 src/$THE_FILE > src/$THE_FILE.new
printf " *	Version: %s\n" "$VERSION" >> src/$THE_FILE.new
tail -n +4 src/$THE_FILE >> src/$THE_FILE.new
mv src/$THE_FILE src/$THE_FILE.old
mv src/$THE_FILE.new src/$THE_FILE
rm src/$THE_FILE.old

# uEliece-utils.h
THE_FILE="uEliece-utils.h"
head -2 src/$THE_FILE > src/$THE_FILE.new
printf " *	Version: %s\n" "$VERSION" >> src/$THE_FILE.new
tail -n +4 src/$THE_FILE >> src/$THE_FILE.new
mv src/$THE_FILE src/$THE_FILE.old
mv src/$THE_FILE.new src/$THE_FILE
rm src/$THE_FILE.old

# uEliece-settings.h
THE_FILE="uEliece-settings.h"
head -2 src/$THE_FILE > src/$THE_FILE.new
printf " *	Version: %s\n" "$VERSION" >> src/$THE_FILE.new
tail -n +4 src/$THE_FILE >> src/$THE_FILE.new
mv src/$THE_FILE src/$THE_FILE.old
mv src/$THE_FILE.new src/$THE_FILE
rm src/$THE_FILE.old
