import os
for NUMT in [1, 8]:
    cmd = "g++ -o project7 project7.cpp -lm -fopenmp -DNUMT=%d" % NUMT
    os.system(cmd)
    cmd = "./project7"
    os.system(cmd)
cmd = "rm -f project7"
os.system(cmd)
