import pickle as pk
import random
import struct

wykladniki = [4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24]
#wykladniki = [26]
maxN = 16000
k = 1
for i in wykladniki:
    n_rekordow = 2**i
    zakres = list(range(1, maxN))
    maxN = maxN*2
    rekordy = [random.choice(zakres) for _ in range(3*n_rekordow)]
    
    with open(f"test{k}", "wb") as f:
        for r in rekordy:
            f.write(struct.pack('<I', r))
    print(f"Zapisano do test{k}")
    k += 1
