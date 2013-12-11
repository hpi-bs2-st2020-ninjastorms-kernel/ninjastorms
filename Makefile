


PREFIX = arm-none-linux-gnueabi-
CC = $(PREFIX)gcc
AR = $(PREFIX)ar
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

SDDEV = /dev/sdd1
SDMNT = /mnt/sd1

LOADADDR = 0xC1000000
ENTRYADDR = 0xC1000004

CFLAGS = -g -Os -fno-common -ffixed-r8 -msoft-float -fno-common -ffixed-r8 -msoft-float -D__KERNEL__ -DTEXT_BASE=0xC1080000 -I./include -fno-builtin -ffreestanding -nostdinc -isystem /usr/local/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3/include -pipe -DCONFIG_ARM -D__ARM__ -marm -mabi=aapcs-linux -mno-thumb-interwork -march=armv5te -march=armv5te -Wall -Wstrict-prototypes -fno-stack-protector -Wall -Wextra -Werror

LDFLAGS = -g -Ttext $(LOADADDR) -L/usr/local/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3 -lgcc 

OBJ = src/ev3ninja.o src/u-boot-stubs.o src/startup.o

ELF  = ev3ninja
BIN  = $(ELF).bin
SREC = $(ELF).srec

.PHONY : all boot.scr boot.cmd deploy clean

all: $(ELF) $(SREC) $(BIN)

$(ELF): $(OBJ)
	@echo "  LD  $@"
	@$(LD) $(LDFLAGS) -o $(ELF) -e $(ELF)_main $(OBJ) 

$(BIN): $(ELF)
	@echo "  OBJCOPY  $(BIN)"
	@$(OBJCOPY) -O binary $(ELF) $(BIN)

$(SREC): $(ELF)
	@echo "  OBJCOPY  $(SREC)"
	@$(OBJCOPY) -O srec $(ELF) $(SREC)

boot.scr: boot.cmd
	@echo "  MKIMAGE  boot.scr"
	@mkimage -C none -A arm -T script -d boot.cmd boot.scr &> /dev/null

boot.cmd:
	@echo "fatload mmc 0 $(LOADADDR) $(BIN)" > boot.cmd
	@echo "go $(ENTRYADDR)" >> boot.cmd

deploy: $(BIN) boot.scr
	@echo "  INSTALL  $(BIN) -> $(SDDEV)"
	@mkdir -p $(SDMNT)
	@mount $(SDDEV) $(SDMNT)
	@cp $(BIN) $(SDMNT)/
	@cp boot.scr $(SDMNT)/
	@umount $(SDDEV)

clean:
	@rm -f src/*.o src/*.a $(ELF) $(BIN) $(SREC) boot.scr boot.cmd

%.o: %.c
	@echo "  CC  $<"
	@$(CC) $(CFLAGS) -o $@ $< -c

lib%.a: %.o
	@echo "  AR  $@"
	@$(AR) crv $@ $<
