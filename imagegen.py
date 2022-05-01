from math import floor
from PIL import Image
from sys import argv

from matplotlib import image
outdata = bytearray()

if (len(argv) < 2):
    print("Input filename must be specified")
    exit(1)

infile = argv[1]
outfile = argv[2] if len(argv) >= 3 else (infile.split(".")[0] + ".bin")


def outputPixel(img: Image.Image, x, y):
    # R (5 bits)   G (6 bits)  B (5 bits)
    # - - - - - | - - - - - - | - - - - -
    pa, pb = 0x00, 0x00
    imagePixel = img.getpixel((x, y))
    r = floor((imagePixel[0]/255.0) * 31.99)
    g = floor((imagePixel[1]/255.0) * 63.99)
    b = floor((imagePixel[2]/255.0) * 31.99)

    pa = (r & 0b11111000) | ((g & 0b111000) >> 3)
    pb = ((g & 0b111) << 5) | (b)
    outdata.append(pa)
    outdata.append(pb)


img = Image.open(infile)
img = img.convert("RGB")
for y in range(0, img.height):
    for x in range(0, img.width):
        outputPixel(img, x, y)


with open(outfile, "wb") as f:
    f.write(outdata)
