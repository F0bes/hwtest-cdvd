EE_OBJS	= dance_factory.o irx/bdm_irx.o irx/bdmfs_vfat_irx.o irx/usbd_irx.o irx/usbmass_bd_irx.o
EE_BIN = dance_factory.elf
EE_LIBS = -lkernel -ldebug -lgraph -lpatches
EE_CFLAGS = -Werror

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS) 

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

emu: $(EE_BIN)
	$(PCSX2) --elf="$(shell pwd))\$(EE_BIN)" 

wsl: $(EE_BIN)
	$(PCSX2) --elf="$(shell wslpath -w $(shell pwd))\$(EE_BIN)"

reset:
	ps2client reset
	ps2client netdump

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
