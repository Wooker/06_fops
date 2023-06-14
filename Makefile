obj-m += solution.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo insmod solution.ko a=0
	sudo dmesg -c

unload:
	sudo rmmod solution.ko
	sudo dmesg -c

