CC = gcc
CFLAGS = -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
CDEFINES = -DBUILDTYPE_X64_LINUX
KECCAK = Keccak-compact
BUILDTYPE = x64-linux
VERSION = $(shell cat ../version)
 
all: build/libuEliece.a

build/libuEliece.a: build/uEliece.o build/uEliece_Keccak.o
	-$(RM) build/libuEliece.a
	ar rcs build/libuEliece.a build/uEliece.o build/uEliece_Keccak.o

build/uEliece.o: ../src/uEliece.c ../src/uEliece.h ../src/uEliece-settings.h
	mkdir -p build
	$(CC) $(CFLAGS) $(CDEFINES) -c -o build/uEliece.o ../src/uEliece.c

build/uEliece_Keccak.o: ../src/Keccak/$(KECCAK).c ../src/Keccak/$(KECCAK).h ../src/Keccak/$(KECCAK)-settings.h ../src/Keccak/crypto_hash.h
	mkdir -p build
	$(CC) $(CFLAGS) $(CDEFINES) -c -o build/uEliece_Keccak.o ../src/Keccak/$(KECCAK).c
	
clean:
	$(RM) -rf build *~

release:
	mkdir -p ../release/$(BUILDTYPE)
	cp build/libuEliece.a ../release/$(BUILDTYPE)/libuEliece-$(VERSION).a
	
