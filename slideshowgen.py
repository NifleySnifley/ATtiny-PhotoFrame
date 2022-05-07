import curses
import pathlib
import random

from cv2 import add

from imagegen import imageToByteArray
import os
import sys

if (len(sys.argv) < 2):
    print("Error, slideshow directory must be provided in argument 1")
    exit(1)
slideshow_dir = sys.argv[1]

output_file = "slideshow.bin"
if (len(sys.argv) >= 3):
    output_file = sys.argv[2]

images = []
for file in os.listdir(slideshow_dir):
    if (file.startswith(".") or not file.endswith(".jpeg")):
        continue
    images.append(str(pathlib.Path(slideshow_dir).joinpath(file)))

images = random.sample(images, k=256)
images[0] = "./happymothersday.png"
binary = bytearray()
curSector = 0

# First two sectors contain image pointers
for x in range(512*2):
    binary.append(0x00)
curSector += 2

for i in range(len(images)):
    # Write to address table
    addr = curSector
    # binary[i*2] = (addr & 0xFF)  # LSB first
    # binary[i*2+1] = (addr >> 8) & 0xFF  # Then MSB
    addr_bytes = addr.to_bytes(4, byteorder="little", signed=False)
    binary[i*4+0] = addr_bytes[0]   # LSB first
    binary[i*4+1] = addr_bytes[1]
    binary[i*4+2] = addr_bytes[2]
    binary[i*4+3] = addr_bytes[3]

    data, sectors = imageToByteArray(images[i], 128)
    # Add a one-sector buffer
    for _ in range(512):
        data.append(0x00)
    curSector += sectors + 1

    print(f"Formatting {images[i]}... {sectors} sectors ({curSector})")

    binary += data

with open(output_file, 'wb') as f:
    f.write(binary)
