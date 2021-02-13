#!/bin/bash

# Note the relative paths below -- this needs to be called from inside condor/

tar -cvzf package.tar.gz ../setup.sh ../bin ../lib ../data ../configs
