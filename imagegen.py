from math import floor
from PIL import Image
from sys import argv

from matplotlib import image

if (len(argv) < 2):
    print("Input filename must be specified")
    exit(1)

infile = argv[1]
outfile = argv[2] if len(argv) >= 3 else (infile.split(".")[1][1:] + ".bin")


# Pixels per sector, <pps> pixels are wrote and then the rest of the sector (512 bytes) is filled with ones
pps = 128
sectorPixelCount = 0


def outputPixel(img: Image.Image, x, y, outdata):
    global sectorPixelCount

    # R (5 bits)   G (6 bits)  B (5 bits)
    # - - - - - | - - - - - - | - - - - -
    pa, pb = 0x00, 0x00
    imagePixel = img.getpixel((x, y))
    r = floor((imagePixel[0]/255.0) * 31.99)
    g = floor((imagePixel[1]/255.0) * 63.99)
    b = floor((imagePixel[2]/255.0) * 31.99)

    pa = ((r & 0b11111) << 3) | ((g & 0b111000) >> 3)
    pb = ((g & 0b111) << 5) | (b & 0b11111)
    outdata.append(pb)
    outdata.append(pa)
    sectorPixelCount += 1
    if (sectorPixelCount == pps):
        sectorPixelCount = 0
        for i in range(512 - pps*2):
            outdata.append(0xFF)


dout = bytearray()
img = Image.open(infile)
img = img.convert("RGB")
for y in range(0, img.height):
    for x in range(0, img.width):
        outputPixel(img, x, y, dout)


with open(outfile, "wb") as f:
    f.write(dout)
