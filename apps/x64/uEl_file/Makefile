CC = gcc

CFLAGS  = -DBUILDTYPE_X64_LINUX -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s

TARGET = uEl-file

all: build/$(TARGET)

build/$(TARGET): build/$(TARGET).o
	$(CC) $(CFLAGS) -o build/$(TARGET) build/$(TARGET).o ../../../libuEliece/release/x64-linux/libuEliece.a

build/$(TARGET).o: $(TARGET).c uEl_keys.h
	mkdir -p build
	$(CC) $(CFLAGS) -c -o build/$(TARGET).o $(TARGET).c

keygen: ../uEliece-keygen/uEl_keygen.cpp
	make -C '../uEliece-keygen'

key:
	../uEliece-keygen/uEl-keygen -C
	echo '#include "../uEliece/uEliece.h"' > uEl_keys.h
	echo 'const uEl_PrivKey private_key = ' >> uEl_keys.h
	cat uEl_priv.key >> uEl_keys.h
	echo ';' >> uEl_keys.h
	echo '' >> uEl_keys.h
	echo 'uEl_PubKey public_key = ' >> uEl_keys.h	
	cat uEl_pub.key >> uEl_keys.h
	echo ';' >> uEl_keys.h
	$(RM) uEl_pub.key uEl_priv.key

clean:
	$(RM) -rf build/*.o *~
