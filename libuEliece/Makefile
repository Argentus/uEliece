VERSION = $(shell cat version)

help:
	@echo ====================================================
	@echo = libuEliece: version '#'$(VERSION)
	@echo =
	@echo = Use "\"make <platform>\""
	@echo = Available platforms: x64-linux, x64-windows, avr8
	@echo = To build for all platforms: "\"make all\""
	@echo =---------------------
	@echo = "\"make clean\"" to clean src and .build directories
	@echo ====================================================

all: x64-linux x64-windows avr8

x64-linux:
	make -C .build-x64-linux

x64-windows:
	make -C .build-x64-windows

avr8:
	make -C .build-avr8

clean-src:
	rm -f src/*~

clean-build:
	make clean -C .build-x64-linux
	make clean -C .build-x64-windows
	make clean -C .build-avr8

clean:	clean-build clean-src
