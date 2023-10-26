import pickle as pk
import random
import struct

#wykladniki = [4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24]
wykladniki = [26]
maxN = 16000
k = 1
for i in wykladniki:
    n_rekordow = 2**i
    zakres = list(range(1, maxN))
    maxN = maxN*2
    rekordy = [random.choice(zakres) for _ in range(3*n_rekordow)]
    
    with open(f"mega{k}", "wb") as f:
        for r in rekordy:
            f.write(struct.pack('<I', r))
    print(f"Zapisano do test{k}")
    k += 1


#n_rekordow = 10
#rekordy = [random.randint(1, 15) for i in range(3*n_rekordow)]

#with open("przyklad", "wb") as f:
#    for r in rekordy:
#        f.write(struct.pack('<I', r))     
#rekordy = [44,55,12,42,94,18,6,67]
#for i in range(0, 3*n_rekordow-2, 3):
#    print(f"{rekordy[i]} {rekordy[i+1]} {rekordy[i+2]}")

 

#all_numbers = list(range(1, 16))

#rekordy = random.sample(all_numbers*3, 3*n_rekordow) 
#rek 10
#rek1  100 
#rek3  10 tys
#rek3 
#rek4
#rek5 100 MLN
#100 000 000
#1 000 000
#10 000
#100
#10