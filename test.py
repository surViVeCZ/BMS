import numpy as np

from pyldpc.code import make_ldpc
from pyldpc import utils
from pyldpc.encoder import encode

text_ascii = "1"
text_bits = [ bin(ord(c))[2:].zfill(8) for c in text_ascii ]
text = ''.join(text_bits)
# cast to int
text = [int(c) for c in text]
# cast to numpy array
text = np.array(text)

n = 2 * len(text)
d_v = len(text) - 1
d_c = len(text) 

H, G = make_ldpc(n, d_v, d_c)

print("'1' in binary: ", text)
print("H:")
print(H)
print("G:")
print(G)

y = encode(G, text, 0)

print("y:")
print(y)
