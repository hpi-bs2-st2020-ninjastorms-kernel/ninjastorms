
## process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = -I./libc/include
AM_CCASFLAGS = $(AM_CFLAGS) $(CFLAGS)
AM_LDFLAGS = --oformat=binary

noinst_LTLIBRARIES = libc/libc.la

libc_libc_la_SOURCES = \
    libc/errno/errno.c \
    libc/stdio/printf.c \
    libc/stdio/putchar.c \
    libc/stdio/puts.c \
    libc/stdio/vprintf.c \
    libc/syscall/syscall.c \
    libc/math/math.c \
    libc/string/memcmp.c \
    libc/string/memcpy.c \
    libc/string/memset.c \
    libc/thread/thread.c \
    libc/include/errno.h \
    libc/include/syscall.h \
    libc/include/math.h \
    libc/include/stddef.h \
    libc/include/stdio.h \
    libc/include/string.h \
    libc/process/process.h \
    libc/include/sys/types.h

noinst_PROGRAMS = ninjastorms

ninjastorms_SOURCES = \
    kernel/boot/start.S \
    kernel/main.c kernel/main.h \
    kernel/tasks.c kernel/tasks.h \
    kernel/scheduler.c kernel/scheduler.h \
    kernel/interrupt.c kernel/interrupt.h \
    kernel/syscall/syscall_handler.S kernel/syscall/syscall_handler.h \
	kernel/syscall/syscall_dispatcher.c kernel/syscall/syscall_dispatcher.h \
    kernel/syscall/inter_process_com.c kernel/syscall/inter_process_com.h \
    kernel/syscall/process_control.c kernel/syscall/process_control.h \
    kernel/utilities.c kernel/utilities.h \
    kernel/interrupt_handler.S kernel/interrupt_handler.h \
    kernel/hal.h \
    kernel/mmio.c kernel/mmio.h \
    kernel/drivers/adc.c kernel/drivers/adc.h \
    kernel/drivers/button.c kernel/drivers/button.h \
    kernel/drivers/gpio.c kernel/drivers/gpio.h \
    kernel/drivers/led.c kernel/drivers/led.h \
    kernel/drivers/motor.c kernel/drivers/motor.h \
    kernel/drivers/pininfo.c kernel/drivers/pininfo.h \
    kernel/drivers/sensor.c kernel/drivers/sensor.h \
    kernel/drivers/uart.c kernel/drivers/uart.h \
    kernel/drivers/spi.c kernel/drivers/spi.h \
    kernel/drivers/timer.c kernel/drivers/timer.h\
    kernel/drivers/interrupt.c kernel/drivers/interrupt.h \
    usermode/init.c usermode/init.h \
    usermode/examples.c usermode/examples.h \
    usermode/utilities.c usermode/utilities.h

ninjastorms_LDADD = libc/libc.la -lgcc
ninjastorms_LDFLAGS = -T kernel/link-arm-eabi.ld -Ttext $(LOADADDR)

all-local: uImage

# workaround for generating plain binaries for arm targets
$(noinst_PROGRAMS).bin: $(noinst_PROGRAMS)
	if file $< | grep executable >/dev/null; then $(STRIP) -O binary -o $<.bin $<; fi

# generate a boot.scr for booting via ymodem download
boot.scr: boot.cmd
	@echo "  MKIMAGE  boot.scr"
	$(Q)mkimage -C none -A arm -T script -d boot.cmd boot.scr

# generate uImage for downloading or booting directly from SD
uImage: $(noinst_PROGRAMS).bin
	@echo "  MKIMAGE  ninjastorms"
	$(Q)mkimage -C none -A arm -T kernel -O linux -a $(LOADADDR) -e $(LOADADDR)  -d $< uImage
