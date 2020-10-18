#!/usr/bin/env python3

from PIL import Image
import sys
import numpy as np

path = sys.argv[1] if len(sys.argv) > 1 else 'image.bmp'

img = Image.open(path)
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
line_bytes = np.zeros(img.size[0], dtype=int)
for i in range(len(line_bytes)):
    for j in range(img.size[1]):
        line_bytes[i] = line_bytes[i] | (bits[i, j] << j)

words = np.zeros(img.size[0] // 4, dtype=int)
for i in range(len(words)):
    for j in range(4):
        words[i] = words[i] | (line_bytes[i * 4 + j] << (8 * j))

s = '{'
for w in words:
    s += hex(w) + ', '
s = s[:-2] + '};'
print(s)
