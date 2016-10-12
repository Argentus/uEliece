CC = gcc
CFLAGS = -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
#CFLAGS = -Wall -g -O0

KECCAK = Keccak-compact
 
all: build/libuEliece.a

build/libuEliece.a: build/uEliece.o build/uEliece_Keccak.o
	-$(RM) build/libuEliece.a
	ar rcs build/libuEliece.a build/uEliece.o build/uEliece_Keccak.o
	cp build/uEliece.a ../release/libuEliece.a

build/uEliece.o: uEliece.c uEliece.h uEliece-settings.h
	mkdir -p build
	$(CC) $(CFLAGS) -c -o build/uEliece.o uEliece.c

build/uEliece_Keccak.o: Keccak/$(KECCAK).c Keccak/$(KECCAK).h Keccak/$(KECCAK)-settings.h Keccak/crypto_hash.h
	mkdir -p build
	$(CC) $(CFLAGS) -c -o build/uEliece_Keccak.o Keccak/$(KECCAK).c
	
clean:
	$(RM) -rf build *~
