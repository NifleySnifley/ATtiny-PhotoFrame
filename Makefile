MCU = 84
COMPILE = avr-gcc -Wall -Os -mmcu=attiny${MCU}
FILENAME = minimac
OUTDIR = ./out

default: compile flash

compile: 
	${COMPILE} -ffunction-sections -fdata-sections -I./TFT  -c ${FILENAME}.c -o ${OUTDIR}/${FILENAME}.o
# 	${COMPILE} -I./ili9341 -c ./ili9341/glcd.c -o glcd.o
#	${COMPILE} -I./ili9341 -c ./ili9341/font.c -o font.o
	${COMPILE} -Wl,-gc-sections -o ${OUTDIR}/${FILENAME}.elf ${OUTDIR}/${FILENAME}.o
	avr-objcopy -j .text -j .data -O ihex ${OUTDIR}/${FILENAME}.elf ${OUTDIR}/${FILENAME}.hex
	avr-size ${OUTDIR}/${FILENAME}.elf
	
flash:
	avrdude -p t${MCU} -P COM18 -c avrisp -b 19200 -U flash:w:${OUTDIR}/${FILENAME}.hex

clean:
	rm ${OUTDIR}/${filename}.elf
	rm ${OUTDIR}/${filename}.o
	rm ${OUTDIR}/${filename}.hex