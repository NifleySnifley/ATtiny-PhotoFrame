MCU = 84
COMPILE = avr-gcc -Wall -Os -mmcu=attiny${MCU}
FILENAME = minimac
OUTDIR = ./out
SRCDIR = ./src

default: compile flash

compile: 
	${COMPILE} -ffunction-sections -fdata-sections -I./src  -c ${SRCDIR}/${FILENAME}.c -o ${OUTDIR}/${FILENAME}.o
# 	${COMPILE} -I./ili9341 -c ./ili9341/glcd.c -o glcd.o
#	${COMPILE} -I./ili9341 -c ./ili9341/font.c -o font.o
	${COMPILE} -Wl,-gc-sections -o ${OUTDIR}/${FILENAME}.elf ${OUTDIR}/${FILENAME}.o
	avr-objcopy -j .text -j .data -O ihex ${OUTDIR}/${FILENAME}.elf ${OUTDIR}/${FILENAME}.hex
	avr-size ${OUTDIR}/${FILENAME}.elf
	
flash:
	avrdude -p t${MCU} -P COM18 -c avrisp -b 19200 -U flash:w:${OUTDIR}/${FILENAME}.hex

clean:
	rm ${OUTDIR}/${FILENAME}.elf
	rm ${OUTDIR}/${FILENAME}.o
	rm ${OUTDIR}/${FILENAME}.hex