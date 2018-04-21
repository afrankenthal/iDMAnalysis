#!/usr/bin/env python

#fn = 'test.txt'

import os

def addprefix(fn):
    f = open(fn+'.tmp', 'w')
    inf = open(fn, 'r')
    for line in inf:
        f.write('file:/eos/uscms/store/user/wsi/standaloneComp/'+line)
    inf.close()
    f.close()
    
    os.system('mv {0}.tmp {0}'.format(fn))

for f in os.listdir('.'):
    if f.endswith('.txt'):
    #if f.endswith('100cm.txt'):
        print f
        addprefix(f)
print "Done."
