COMPILE = avr-gcc -Wall -Os -mmcu=attiny48
FILENAME = minimac

default: compile clean flash

compile: 
	${COMPILE} -ffunction-sections -fdata-sections -I./TFT  -c ${FILENAME}.c -o ${FILENAME}.o
# 	${COMPILE} -I./ili9341 -c ./ili9341/glcd.c -o glcd.o
#	${COMPILE} -I./ili9341 -c ./ili9341/font.c -o font.o
	${COMPILE} -Wl,-gc-sections -o ${FILENAME}.elf ${FILENAME}.o
	avr-objcopy -j .text -j .data -O ihex ${FILENAME}.elf ${FILENAME}.hex
	avr-size ${FILENAME}.elf
	
flash:
	avrdude -p t48 -P COM18 -c avrisp -b 19200 -U flash:w:${FILENAME}.hex

clean:
	rm *.o *.elf