
all:
	cd ntl-10.3.0/src; ./configure NTL_GMP_LIP=off
	make -C ntl-10.3.0/src/
	g++ -g -O2 uEl_keygen.cpp ./ntl-10.3.0/src/ntl.a -o uEl-keygen -std=gnu++11

clean:
	rm *.o *.key uEl-keygen
