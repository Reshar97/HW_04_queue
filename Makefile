PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
DRV_NAME := ex_queue
obj-m := $(DRV_NAME).o
CC := x86_64-linux-gnu-gcc

.PHONY: build run remove install uninstall clean check format

build:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

run: build
	insmod $(PWD)/$(DRV_NAME).ko

remove:
	rmmod $(DRV_NAME)

install: build
	@echo "Установка модуля..."
	@sudo cp $(PWD)/$(DRV_NAME).ko /lib/modules/$(shell uname -r)/kernel/drivers/misc/
	@sudo depmod -a # Это нужно, чтобы обновить общий список модулей
	@sudo insmod /lib/modules/$(shell uname -r)/kernel/drivers/misc/$(DRV_NAME).ko

uninstall:
	@echo "Удаление модуля..."
	@sudo modprobe -r $(DRV_NAME)
	@sudo rm -f /lib/modules/$(shell uname -r)/kernel/drivers/misc/$(DRV_NAME).ko
	@sudo depmod -a

clean: 
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

format:
	clang-format -i $(DRV_NAME).c

check: build run remove
	dmesg | tail -n 40 # Покажем больше строк для контекста

all: build
