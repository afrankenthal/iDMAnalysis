## Run with python mergeCRABOutput.sh, instead of ./mergeCRABOutput.py
## This guarantees the right python from the CMSSW release is called

from CRABAPI.RawCommand import crabCommand
from CRABClient.UserUtilities import setConsoleLogLevel
from CRABClient.ClientUtilities import LOGLEVEL_MUTE
setConsoleLogLevel(LOGLEVEL_MUTE)
import sys

# import os
# cwd = os.getcwd()
# print "Current directory: ", cwd

if len(sys.argv) > 1:
    res = crabCommand('status', dir = sys.argv[1])
# else:
    # res = crabCommand('status')

#if res['status'] == 'COMPLETED':
userWebDirURL = res['userWebDirURL']
datestamp = userWebDirURL.split(':')[-2].split('/')[-1]
print datestamp
    #exit(0)
# else:
    # exit(1)
