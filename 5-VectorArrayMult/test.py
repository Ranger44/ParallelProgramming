import os
for ARRAYSIZE in [1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216, 33554432]:
    cmd = "g++  -c simd.p5.cpp -o simd.p5.o"
    os.system( cmd )
    cmd = "g++ -o runProg proj5.cpp simd.p5.o -DNUMTIMES=100 -DARRAYSIZE=%d -lm -fopenmp" % ARRAYSIZE
    os.system( cmd )
    cmd = "./runProg"
    os.system( cmd )
cmd = "rm -f runProg"
os.system( cmd )
cmd = "rm -f simd.p5.o"
os.system( cmd )