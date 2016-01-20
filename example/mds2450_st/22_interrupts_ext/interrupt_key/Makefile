obj-m = hwkey.o

all:
	make -C /work/android/kernel M=$(PWD) modules
clean:
	make -C /work/android/kernel M=$(PWD) clean

hwkey:hwkey.c
	$(CROSS_COMPILE)gcc -o hwkey hwkey.c

