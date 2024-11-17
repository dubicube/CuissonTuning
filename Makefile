DEVICE     = atmega328p
CLOCK      = 16000000
#PROGRAMMER = -c avrisp -P /dev/ttyACM0 -b 19200
PROGRAMMER = -c arduino -P /dev/ttyACM0
#PROGRAMMER = -c avrisp -P COM3 -b 19200
#PROGRAMMER = -c usbasp -P COM3 -b 19200
OBJECTS    = PLQ_plaque.c TRX_uart.c LED_ruban.c main.c
#FUSES      = -U lfuse:w:0xff:m -U hfuse:w:0xde:m -U efuse:w:0x05:m
FUSES      = -U lfuse:w:0xe2:m -U hfuse:w:0xde:m -U efuse:w:0xfd:m
#E2

######################################################################
######################################################################

# Tune the lines below only if you know what you are doing:

#AVRDUDE = avrdude -C "C:\Program Files (x86)\Arduino\hardware\tools\avr\etc\avrdude.conf" $(PROGRAMMER) -p $(DEVICE)
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

clean:
	rm -f main.elf

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
