#!/usr/bin/env python3

from PIL import Image
import sys
import numpy as np

path = sys.argv[1] if len(sys.argv) > 1 else 'image.bmp'

img = Image.open(path)

assert (img.size[0] % 8 == 0)
assert (img.size[1] % 8 == 0)
ntiles = (img.size[0] // 8, img.size[1] // 8)
ntiles_total = ntiles[0] * ntiles[1]

pixels = img.load()

bits = np.zeros(img.size, dtype=bool)

# Write the bits
for i in range(img.size[0]):
    for j in range(img.size[1]):
        p = pixels[i, j]
        if p == (255, 255, 255):
            bits[i, j] = True
        elif p == (0, 0, 0):
            bits[i, j] = False
        else:
            raise RuntimeError(f'Wrong pixel: {p}')

# Write the bytes for each line
line_bytes = np.zeros(ntiles_total * 8, dtype=int)
for tilej in range(ntiles[1]):
    for tilei in range(ntiles[0]):
        for i in range(8):
            for j in range(8):
                posi = 8 * tilei + i
                posj = 8 * tilej + j
                line_bytes[tilej * (8 * ntiles[0]) + tilei * 8 + j] = line_bytes[tilej * (8 * ntiles[0]) + tilei * 8 + j] | (bits[posi, posj] << i)

                
words = np.zeros(ntiles_total * 2, dtype=int)
for i in range(ntiles_total * 2):
    for j in range(4):
        words[i] = words[i] | (line_bytes[i * 4 + j] << (8 * j))

s = '{'
for w in words:
    s += hex(w) + ', '
s = s[:-2] + '};'
print(s)
