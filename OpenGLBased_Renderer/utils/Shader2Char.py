#20140125 Cui.Yingyun
#Convert C style code to Char style

import sys

file = sys.stdin
if len(sys.argv) > 1:
  file = open(sys.argv[1])
else:
    print "Input argument which specify shader program"
    sys.exit(0);

lines = file.readlines()
print '\"\\'
for line in lines[:-1]:
  print line.rstrip() + '\\n\\'
print lines[-1].rstrip() + '\\n\"'

file.close()
