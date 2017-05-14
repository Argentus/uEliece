CC = gcc

CFLAGS  = -DBUILDTYPE_X64_LINUX -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s

TARGET = uEl_cli

all: build/$(TARGET)

build/$(TARGET): build/$(TARGET).o
	$(CC) $(CFLAGS) -o build/$(TARGET) build/$(TARGET).o ../../../libuEliece/release/x64-linux/libuEliece.a

build/$(TARGET).o: $(TARGET).c
	mkdir -p build
	$(CC) $(CFLAGS) -c -o build/$(TARGET).o $(TARGET).c

keygen: ../uEliece-keygen/uEl_keygen.cpp
	make -C '../uEliece-keygen'

key:
	../uEliece-keygen/uEl-keygen -B

clean:
	$(RM) -rf build/*.o *~
