CC = avr-gcc

CFLAGS  = -Wall -Os -mmcu=atmega1284p -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s

TARGET = main

all: build/$(TARGET)

build/$(TARGET): build/$(TARGET).o ../../../libuEliece/AVR8/build/uEliece.o ../../../libuEliece/AVR8/build/uEliece_Keccak.o
	mkdir -p release
	$(CC) $(CFLAGS) -Wl,-Map,main.map -o release/$(TARGET).elf 'build/$(TARGET).o' '../../../libuEliece/AVR8/build/uEliece.o' '../../../libuEliece/AVR8/build/uEliece_Keccak.o'

build/$(TARGET).o: $(TARGET).c uEl_keys.h
	mkdir -p build
	$(CC) $(CFLAGS) -c -o build/$(TARGET).o $(TARGET).c

clean:
	$(RM) build/*.o *~

flash:
	avr-objcopy -j .text -j .data -O ihex release/main.elf release/main.hex
	sudo avrdude -c usbasp -p atmega1284p -U flash:w:release/main.hex



