#
KERNEL_SRC_DIR = ~/socfpga/linux-socfpga/
PWD = $(shell pwd)

obj-m := fpga_leds.o

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
		-C $(KERNEL_SRC_DIR) M=$(PWD) LOCALVERSION= modules

.PHONY: clean
clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
		-C $(KERNEL_SRC_DIR) M=$(PWD) clean

