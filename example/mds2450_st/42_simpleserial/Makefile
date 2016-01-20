obj-m += simpleserial.o

KDIR	:= /work/linux-2.6.29-mc100
#KDIR	:= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
