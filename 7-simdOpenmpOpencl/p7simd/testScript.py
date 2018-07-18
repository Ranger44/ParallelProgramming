import os
cmd = "g++ -c simd.p5.cpp -o simd.p5.o"
os.system(cmd)
cmd = "g++ -o project7 project7.cpp simd.p5.o -lm -fopenmp"
os.system(cmd)
cmd = "./project7"
os.system(cmd)
cmd = "rm -f project7"
os.system(cmd)
cmd = "rm -f simd.p5.o"
os.system(cmd)
